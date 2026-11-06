#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"
#include <algorithm>
#include <cstdlib>
#include <sstream>

static void	sendModeQuery(Server *srv, int fd, const std::string &chan,
	const std::string &nick, const std::string &prefix)
{
	ChannelInfo &c = s_channels[chan];
	std::string	mode = "+";
	if (c.inviteOnly)    mode += "i";
	if (c.topicOps)      mode += "t";
	if (!c.key.empty())  mode += "k";
	if (c.userLimit > 0) mode += "l";
	if (mode == "+")     mode = "";
	srv->sendReply(fd, prefix + " 324 " + nick + " " + chan + " " + mode + "\r\n");
}

static void	applyInviteMode(ChannelInfo &c, bool adding, std::string &modes)
{
	c.inviteOnly = adding;
	modes += std::string(adding ? "+" : "-") + "i";
}

static void	applyTopicOpsMode(ChannelInfo &c, bool adding, std::string &modes)
{
	c.topicOps = adding;
	modes += std::string(adding ? "+" : "-") + "t";
}

static void	applyKeyMode(Server *srv, int fd, const Command &cmd,
	ChannelInfo &c, bool adding, size_t &idx,
	std::string &modes, std::string &params,
	const std::string &nick, const std::string &prefix)
{
	if (adding)
	{
		if (idx >= cmd.args.size())
		{
			srv->sendReply(fd, prefix + " 461 " + nick
				+ " MODE :Not enough parameters\r\n");
			return ;
		}
		c.key = cmd.args[idx++];
		modes += "+k";
		params += " " + c.key;
	}
	else
	{
		c.key = "";
		modes += "-k";
	}
}

static void	applyOpMode(Server *srv, int fd, const Command &cmd,
	ChannelInfo &c, const std::string &chan, bool adding, size_t &idx,
	std::string &modes, std::string &params,
	const std::string &nick, const std::string &prefix,
	const std::map<int, Client> &clients)
{
	if (idx >= cmd.args.size())
	{
		srv->sendReply(fd, prefix + " 461 " + nick
			+ " MODE :Not enough parameters\r\n");
		return ;
	}
	const std::string	&tNick = cmd.args[idx++];
	int	targetFd = findFdByNick(clients, tNick);
	if (targetFd == -1 || !isMember(chan, targetFd))
	{
		srv->sendReply(fd, prefix + " 441 " + nick + " " + tNick
			+ " " + chan + " :They aren't on that channel\r\n");
		return ;
	}
	if (adding)
	{
		if (!isOp(chan, targetFd))
			c.operators.push_back(targetFd);
		modes += "+o";
	}
	else
	{
		std::vector<int>::iterator it = std::find(c.operators.begin(),
			c.operators.end(), targetFd);
		if (it != c.operators.end())
			c.operators.erase(it);
		modes += "-o";
	}
	params += " " + tNick;
}

static void	applyLimitMode(Server *srv, int fd, const Command &cmd,
	ChannelInfo &c, bool adding, size_t &idx,
	std::string &modes, std::string &params,
	const std::string &nick, const std::string &prefix)
{
	if (adding)
	{
		if (idx >= cmd.args.size())
		{
			srv->sendReply(fd, prefix + " 461 " + nick
				+ " MODE :Not enough parameters\r\n");
			return ;
		}
		int limit = std::atoi(cmd.args[idx++].c_str());
		if (limit <= 0)
			return ;
		c.userLimit = limit;
		std::ostringstream oss;
		oss << c.userLimit;
		modes += "+l";
		params += " " + oss.str();
	}
	else
	{
		c.userLimit = 0;
		modes += "-l";
	}
}

static void	processModeString(Server *srv, int fd, const Command &cmd,
	const std::string &chan, const std::string &modeStr,
	const std::string &nick, const std::string &prefix,
	const std::map<int, Client> &clients)
{
	ChannelInfo &c = s_channels[chan];
	size_t	idx = 2;
	bool	adding = true;
	std::string	modes = "";
	std::string	params = "";

	for (size_t i = 0; i < modeStr.size(); i++)
	{
		char m = modeStr[i];
		if (m == '+') { adding = true;  continue ; }
		if (m == '-') { adding = false; continue ; }
		else if (m == 'i') applyInviteMode(c, adding, modes);
		else if (m == 't') applyTopicOpsMode(c, adding, modes);
		else if (m == 'k') applyKeyMode(srv, fd, cmd, c, adding, idx, modes, params, nick, prefix);
		else if (m == 'o') applyOpMode(srv, fd, cmd, c, chan, adding, idx, modes, params, nick, prefix, clients);
		else if (m == 'l') applyLimitMode(srv, fd, cmd, c, adding, idx, modes, params, nick, prefix);
		else
			srv->sendReply(fd, prefix + " 472 " + nick + " " + m
				+ " :is unknown mode char to me\r\n");
	}
	if (!modes.empty())
	{
		std::string	modeMsg = userPrefix(fd, clients) + " MODE "
			+ chan + " " + modes + params + "\r\n";
		broadcastToChannel(srv, chan, modeMsg);
	}
}

void	handleMode(Server *srv, int fd, const Command &cmd,
			const std::map<int, Client> &clients)
{
	const std::string	prefix = ":ircserv";
	const std::string	&nick = clients.at(fd).getNickname();

	if (!requireRegistered(srv, fd, nick, prefix))			return ;
	if (!requireMinArgs(srv, fd, cmd, 1, "MODE", nick, prefix))	return ;

	const std::string	&chan = cmd.args[0];
	if (!requireChannelExists(srv, fd, chan, nick, prefix))	return ;

	if (cmd.args.size() == 1)
	{
		sendModeQuery(srv, fd, chan, nick, prefix);
		return ;
	}
	if (!requireMember(srv, fd, chan, nick, prefix))	return ;
	if (!requireOp(srv, fd, chan, nick, prefix))		return ;

	processModeString(srv, fd, cmd, chan, cmd.args[1], nick, prefix, clients);
}

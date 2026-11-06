#include "Helpers.hpp"
#include <algorithm>
#include <sstream>

bool	requireRegistered(Server *srv, int fd, const std::string &nick,
						const std::string &prefix)
{
	if (s_clients[fd].registered)
		return (true);
	srv->sendReply(fd, prefix + " 451 " + nick + " :You have not registered\r\n");
	return (false);
}

bool	requireAuthenticated(Server *srv, int fd, const std::string &prefix)
{
	if (s_clients[fd].authenticated)
		return (true);
	srv->sendReply(fd, prefix + " 451 * :You have not registered\r\n");
	return (false);
}

bool	requireNotRegistered(Server *srv, int fd, const std::string &prefix)
{
	if (!s_clients[fd].registered)
		return (true);
	srv->sendReply(fd, prefix + " 462 * :You may not reregister\r\n");
	return (false);
}

bool	requireMinArgs(Server *srv, int fd, const Command &cmd, size_t min,
					const std::string &cmdName, const std::string &nick,
					const std::string &prefix)
{
	if (cmd.args.size() >= min)
		return (true);
	srv->sendReply(fd, prefix + " 461 " + nick + " " + cmdName
		+ " :Not enough parameters\r\n");
	return (false);
}

bool	requireChannelExists(Server *srv, int fd, const std::string &chan,
						const std::string &nick, const std::string &prefix)
{
	if (channelExists(chan))
		return (true);
	srv->sendReply(fd, prefix + " 403 " + nick + " " + chan
		+ " :No such channel\r\n");
	return (false);
}

bool	requireMember(Server *srv, int fd, const std::string &chan,
					const std::string &nick, const std::string &prefix)
{
	if (isMember(chan, fd))
		return (true);
	srv->sendReply(fd, prefix + " 442 " + nick + " " + chan
		+ " :You're not on that channel\r\n");
	return (false);
}

bool	requireOp(Server *srv, int fd, const std::string &chan,
				const std::string &nick, const std::string &prefix)
{
	if (isOp(chan, fd))
		return (true);
	srv->sendReply(fd, prefix + " 482 " + nick + " " + chan
		+ " :You're not channel operator\r\n");
	return (false);
}

void	broadcastToChannel(Server *srv, const std::string &chan,
						const std::string &msg)
{
	ChannelInfo &c = s_channels[chan];
	for (size_t i = 0; i < c.members.size(); i++)
		srv->sendReply(c.members[i], msg);
}

void	broadcastToChannelExcept(Server *srv, const std::string &chan,
							int exceptFd, const std::string &msg)
{
	ChannelInfo &c = s_channels[chan];
	for (size_t i = 0; i < c.members.size(); i++)
	{
		if (c.members[i] != exceptFd)
			srv->sendReply(c.members[i], msg);
	}
}

void	removeMemberFromChannel(int fd, const std::string &chan)
{
	ChannelInfo &c = s_channels[chan];
	std::vector<int>::iterator it = std::find(c.members.begin(), c.members.end(), fd);
        if (it != c.members.end())
            c.members.erase(it);

}

void	removeOpFromChannel(int fd, const std::string &chan)
{
	ChannelInfo &c = s_channels[chan];
	std::vector<int>::iterator it = std::find(c.operators.begin(),
		c.operators.end(), fd);
	if (it != c.operators.end())
		c.operators.erase(it);
}

void	deleteChannelIfEmpty(const std::string &chan)
{
	if (s_channels[chan].members.empty())
		s_channels.erase(chan);
}

void	leaveChannel(int fd, const std::string &chan)
{
	removeMemberFromChannel(fd, chan);
	removeOpFromChannel(fd, chan);
	deleteChannelIfEmpty(chan);
}

std::vector<std::string>	splitByComma(const std::string &input)
{
	std::vector<std::string>	out;
	std::stringstream			ss(input);
	std::string					token;
	while (std::getline(ss, token, ','))
		out.push_back(token);
	return (out);
}

void	sendWelcomeBurst(Server *srv, int fd, const std::string &nick,
						const std::string &prefix)
{
	srv->sendReply(fd, prefix + " 001 " + nick
		+ " :Welcome to the IRC Network " + nick + "!\r\n");
	srv->sendReply(fd, prefix + " 002 " + nick
		+ " :Your host is ircserv\r\n");
	srv->sendReply(fd, prefix + " 003 " + nick
		+ " :This server was created today\r\n");
	srv->sendReply(fd, prefix + " 004 " + nick
		+ " ircserv 1.0 o itkol\r\n");
}

#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"
#include <algorithm>

static bool	passesInviteOnly(Server *srv, int fd, const std::string &chan,
	const std::string &nick, const std::string &prefix)
{
	ChannelInfo &c = s_channels[chan];
	if (!c.inviteOnly)
		return (true);
	std::vector<int>::iterator it = std::find(c.invited.begin(),
		c.invited.end(), fd);
	if (it != c.invited.end())
	{
		c.invited.erase(it);
		return (true);
	}
	srv->sendReply(fd, prefix + " 473 " + nick + " " + chan
		+ " :Cannot join channel (+i)\r\n");
	return (false);
}

static bool	passesKeyCheck(Server *srv, int fd, const std::string &chan,
	const std::string &key, const std::string &nick, const std::string &prefix)
{
	ChannelInfo &c = s_channels[chan];
	if (c.key.empty() || c.key == key)
		return (true);
	srv->sendReply(fd, prefix + " 475 " + nick + " " + chan
		+ " :Cannot join channel (+k)\r\n");
	return (false);
}

static bool	passesLimitCheck(Server *srv, int fd, const std::string &chan,
	const std::string &nick, const std::string &prefix)
{
	ChannelInfo &c = s_channels[chan];
	if (c.userLimit == 0 || (int)c.members.size() < c.userLimit)
		return (true);
	srv->sendReply(fd, prefix + " 471 " + nick + " " + chan
		+ " :Cannot join channel (+l)\r\n");
	return (false);
}

static void	createChannel(const std::string &chan)
{
	if (channelExists(chan))
		return ;
	ChannelInfo nc;
	nc.userLimit = 0;
	nc.inviteOnly = false;
	nc.topicOps = false;
	s_channels[chan] = nc;
}

static void	sendNamesReply(Server *srv, int fd, const std::string &chan,
	const std::string &nick, const std::string &prefix,
	const std::map<int, Client> &clients)
{
	ChannelInfo &c = s_channels[chan];
	std::string	names = "";
	for (size_t i = 0; i < c.members.size(); i++)
	{
		int mfd = c.members[i];
		if (isOp(chan, mfd))
			names += "@";
		if (clients.find(mfd) != clients.end())
			names += clients.at(mfd).getNickname();
		if (i + 1 < c.members.size())
			names += " ";
	}
	srv->sendReply(fd, prefix + " 353 " + nick + " = " + chan
		+ " :" + names + "\r\n");
	srv->sendReply(fd, prefix + " 366 " + nick + " " + chan
		+ " :End of /NAMES list\r\n");
}

void	handleJoin(Server *srv, int fd, const Command &cmd,
			const std::map<int, Client> &clients)
{
	const std::string	prefix = ":ircserv";
	const std::string	&nick = clients.at(fd).getNickname();

	if (!requireRegistered(srv, fd, nick, prefix))			return ;
	if (!requireMinArgs(srv, fd, cmd, 1, "JOIN", nick, prefix))	return ;

	std::vector<std::string>	chans = splitByComma(cmd.args[0]);
	std::vector<std::string>	keys;
	if (cmd.args.size() > 1)
		keys = splitByComma(cmd.args[1]);

	for (size_t i = 0; i < chans.size(); i++)
	{
		const std::string	&chan = chans[i];
		std::string	key = (i < keys.size()) ? keys[i] : "";

		if (chan.empty() || chan[0] != '#')
		{
			srv->sendReply(fd, prefix + " 403 " + nick + " " + chan
				+ " :No such channel\r\n");
			continue ;
		}
		if (isMember(chan, fd))
			continue ;
		if (channelExists(chan))
		{
			if (!passesInviteOnly(srv, fd, chan, nick, prefix))	continue ;
			if (!passesKeyCheck(srv, fd, chan, key, nick, prefix))	continue ;
			if (!passesLimitCheck(srv, fd, chan, nick, prefix))	continue ;
		}

		createChannel(chan);
		ChannelInfo &c = s_channels[chan];
		c.members.push_back(fd);
		if (c.members.size() == 1)
			c.operators.push_back(fd);

		std::string	joinMsg = userPrefix(fd, clients) + " JOIN " + chan + "\r\n";
		broadcastToChannel(srv, chan, joinMsg);

		if (!c.topic.empty())
			srv->sendReply(fd, prefix + " 332 " + nick + " " + chan
				+ " :" + c.topic + "\r\n");
		else
			srv->sendReply(fd, prefix + " 331 " + nick + " " + chan
				+ " :No topic is set\r\n");

		sendNamesReply(srv, fd, chan, nick, prefix, clients);
	}
}

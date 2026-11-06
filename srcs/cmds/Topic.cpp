#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"

static bool	topicOpsRestriction(Server *srv, int fd, const std::string &chan,
	const std::string &nick, const std::string &prefix)
{
	if (!s_channels[chan].topicOps || isOp(chan, fd))
		return (true);
	srv->sendReply(fd, prefix + " 482 " + nick + " " + chan
		+ " :You're not channel operator\r\n");
	return (false);
}

static void	sendTopicQuery(Server *srv, int fd, const std::string &chan,
	const std::string &nick, const std::string &prefix)
{
	if (s_channels[chan].topic.empty())
		srv->sendReply(fd, prefix + " 331 " + nick + " " + chan
			+ " :No topic is set\r\n");
	else
		srv->sendReply(fd, prefix + " 332 " + nick + " " + chan
			+ " :" + s_channels[chan].topic + "\r\n");
}

void	handleTopic(Server *srv, int fd, const Command &cmd,
			const std::map<int, Client> &clients)
{
	const std::string	prefix = ":ircserv";
	const std::string	&nick = clients.at(fd).getNickname();

	if (!requireRegistered(srv, fd, nick, prefix))			return ;
	if (!requireMinArgs(srv, fd, cmd, 1, "TOPIC", nick, prefix))	return ;

	const std::string	&chan = cmd.args[0];
	if (!requireChannelExists(srv, fd, chan, nick, prefix))	return ;
	if (!requireMember(srv, fd, chan, nick, prefix))		return ;

	if (cmd.args.size() == 1)
	{
		sendTopicQuery(srv, fd, chan, nick, prefix);
		return ;
	}

	if (!topicOpsRestriction(srv, fd, chan, nick, prefix))	return ;

	s_channels[chan].topic = cmd.args[1];
	std::string	topicMsg = userPrefix(fd, clients) + " TOPIC "
		+ chan + " :" + s_channels[chan].topic + "\r\n";
	broadcastToChannel(srv, chan, topicMsg);
}

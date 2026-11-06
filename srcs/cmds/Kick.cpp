#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"

static bool	kickTargetIsMember(Server *srv, int fd, const std::string &chan,
	const std::string &targetNick, const std::string &nick,
	const std::string &prefix, const std::map<int, Client> &clients,
	int &targetFd)
{
	targetFd = findFdByNick(clients, targetNick);
	if (targetFd != -1 && isMember(chan, targetFd))
		return (true);
	srv->sendReply(fd, prefix + " 441 " + nick + " " + targetNick + " "
		+ chan + " :They aren't on that channel\r\n");
	return (false);
}

void	handleKick(Server *srv, int fd, const Command &cmd,
			const std::map<int, Client> &clients)
{
	const std::string	prefix = ":ircserv";
	const std::string	&nick = clients.at(fd).getNickname();

	if (!requireRegistered(srv, fd, nick, prefix))				return ;
	if (!requireMinArgs(srv, fd, cmd, 2, "KICK", nick, prefix))	return ;

	const std::string	&chanName = cmd.args[0];
	const std::string	&targetNick = cmd.args[1];
	std::string			reason = (cmd.args.size() > 2) ? cmd.args[2] : nick;

	if (!requireChannelExists(srv, fd, chanName, nick, prefix))	return ;
	if (!requireMember(srv, fd, chanName, nick, prefix))		return ;
	if (!requireOp(srv, fd, chanName, nick, prefix))			return ;

	int	targetFd = -1;
	if (!kickTargetIsMember(srv, fd, chanName, targetNick, nick, prefix, clients, targetFd))
		return ;

	std::string kickMsg = userPrefix(fd, clients) + " KICK "
		+ chanName + " " + targetNick + " :" + reason + "\r\n";
	broadcastToChannel(srv, chanName, kickMsg);
	leaveChannel(targetFd, chanName);
}

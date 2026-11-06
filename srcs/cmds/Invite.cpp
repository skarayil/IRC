#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"

static bool	inviteTargetExists(Server *srv, int fd, const std::string &target,
	const std::string &sender, const std::string &prefix,
	const std::map<int, Client> &clients, int &targetFd)
{
	targetFd = findFdByNick(clients, target);
	if (targetFd != -1)
		return (true);
	srv->sendReply(fd, prefix + " 401 " + sender + " " + target
		+ " :No such nick/channel\r\n");
	return (false);
}

static bool	inviteTargetNotMember(Server *srv, int fd, const std::string &chan,
	const std::string &target, const std::string &sender,
	const std::string &prefix, int targetFd)
{
	if (!isMember(chan, targetFd))
		return (true);
	srv->sendReply(fd, prefix + " 443 " + sender + " " + target + " "
		+ chan + " :is already on channel\r\n");
	return (false);
}

void	handleInvite(Server *srv, int fd, const Command &cmd,
			const std::map<int, Client> &clients)
{
	const std::string	prefix = ":ircserv";
	const std::string	&sender = clients.at(fd).getNickname();

	if (!requireRegistered(srv, fd, sender, prefix))			return ;
	if (!requireMinArgs(srv, fd, cmd, 2, "INVITE", sender, prefix))	return ;

	const std::string	&target = cmd.args[0];
	const std::string	&chan = cmd.args[1];

	if (!requireChannelExists(srv, fd, chan, sender, prefix))	return ;
	if (!requireMember(srv, fd, chan, sender, prefix))			return ;
	if (!requireOp(srv, fd, chan, sender, prefix))				return ;

	int	targetFd = -1;
	if (!inviteTargetExists(srv, fd, target, sender, prefix, clients, targetFd))
		return ;
	if (!inviteTargetNotMember(srv, fd, chan, target, sender, prefix, targetFd))
		return ;

	s_channels[chan].invited.push_back(targetFd);
	srv->sendReply(fd, prefix + " 341 " + sender + " " + target + " " + chan + "\r\n");
	srv->sendReply(targetFd, userPrefix(fd, clients)
		+ " INVITE " + target + " " + chan + "\r\n");
}

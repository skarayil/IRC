#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"

static bool	hasRecipient(Server *srv, int fd, const Command &cmd,
	const std::string &nick, const std::string &prefix)
{
	if (!cmd.args.empty())
		return (true);
	srv->sendReply(fd, prefix + " 411 " + nick
		+ " :No recipient given (PRIVMSG)\r\n");
	return (false);
}

static bool	hasText(Server *srv, int fd, const Command &cmd,
	const std::string &nick, const std::string &prefix)
{
	if (cmd.args.size() >= 2)
		return (true);
	srv->sendReply(fd, prefix + " 412 " + nick + " :No text to send\r\n");
	return (false);
}

static bool	targetExists(Server *srv, int fd, const std::string &target,
	const std::string &nick, const std::string &prefix,
	const std::map<int, Client> &clients, int &targetFd)
{
	targetFd = findFdByNick(clients, target);
	if (targetFd != -1)
		return (true);
	srv->sendReply(fd, prefix + " 401 " + nick + " " + target
		+ " :No such nick/channel\r\n");
	return (false);
}

static void	sendToChannel(Server *srv, int fd, const std::string &target,
	const std::string &msg, const std::map<int, Client> &clients)
{
	std::string	full = userPrefix(fd, clients) + " PRIVMSG "
		+ target + " :" + msg + "\r\n";
	broadcastToChannelExcept(srv, target, fd, full);
}

void	handlePrivmsg(Server *srv, int fd, const Command &cmd,
			const std::map<int, Client> &clients)
{
	const std::string	prefix = ":ircserv";
	const std::string	&nick = clients.at(fd).getNickname();

	if (!requireRegistered(srv, fd, nick, prefix))	return ;
	if (!hasRecipient(srv, fd, cmd, nick, prefix))	return ;
	if (!hasText(srv, fd, cmd, nick, prefix))		return ;

	const std::string	&target = cmd.args[0];
	const std::string	&msg = cmd.args[1];

	if (!target.empty() && target[0] == '#')
	{
		if (!requireChannelExists(srv, fd, target, nick, prefix))	return ;
		if (!requireMember(srv, fd, target, nick, prefix))			return ;
		sendToChannel(srv, fd, target, msg, clients);
		if (msg == "!joke" || msg == "!espri")
		{
			std::string joke = getRandomJoke();
			broadcastToChannel(srv, target, ":JokeBot!bot@localhost PRIVMSG " + target + " :" + joke + "\r\n");
		}
	}
	else
	{
		if (target == "JokeBot" || target == "jokebot")
		{
			std::string joke = getRandomJoke();
			srv->sendReply(fd, ":JokeBot!bot@localhost PRIVMSG " + nick + " :" + joke + "\r\n");
			return ;
		}
		int	targetFd = -1;
		if (!targetExists(srv, fd, target, nick, prefix, clients, targetFd))
			return ;
		srv->sendReply(targetFd, userPrefix(fd, clients)
			+ " PRIVMSG " + target + " :" + msg + "\r\n");
	}
}

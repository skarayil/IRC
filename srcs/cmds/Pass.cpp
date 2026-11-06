#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"

static bool	isPasswordCorrect(Server *srv, int fd, const std::string &given,
	const std::string &serverPass, const std::string &prefix)
{
	if (given == serverPass)
		return (true);
	srv->sendReply(fd, prefix + " 464 * :Password incorrect\r\n");
	return (false);
}

void	handlePass(Server *srv, int fd, const Command &cmd,
			const std::string &serverPass)
{
	const std::string	prefix = ":ircserv";

	if (!requireNotRegistered(srv, fd, prefix))			return ;
	if (!requireMinArgs(srv, fd, cmd, 1, "PASS", "*", prefix))	return ;
	if (!isPasswordCorrect(srv, fd, cmd.args[0], serverPass, prefix))	return ;

	s_clients[fd].authenticated = true;
}

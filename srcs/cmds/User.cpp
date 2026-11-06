#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"

void	handleUser(Server *srv, int fd, const Command &cmd,
			const std::map<int, Client> &clients)
{
	const std::string	prefix = ":ircserv";

	if (!requireAuthenticated(srv, fd, prefix))			return ;
	if (!requireNotRegistered(srv, fd, prefix))			return ;
	if (!requireMinArgs(srv, fd, cmd, 4, "USER", "*", prefix))	return ;

	s_clients[fd].username = cmd.args[0];

	const std::string	&nick = clients.at(fd).getNickname();
	if (!nick.empty())
	{
		s_clients[fd].registered = true;
		sendWelcomeBurst(srv, fd, nick, prefix);
	}
}

#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"

static bool	isNickAvailable(Server *srv, int fd, const std::string &newNick,
	const std::string &prefix, const std::map<int, Client> &clients)
{
	int	existing = findFdByNick(clients, newNick);
	if (existing == -1 || existing == fd)
		return (true);
	srv->sendReply(fd, prefix + " 433 * " + newNick
		+ " :Nickname is already in use\r\n");
	return (false);
}

void	handleNick(Server *srv, int fd, const Command &cmd,
			std::map<int, Client> &clients)
{
	const std::string	prefix = ":ircserv";

	if (!requireAuthenticated(srv, fd, prefix))				return ;
	if (cmd.args.empty())
	{
		std::string currentNick = clients[fd].getNickname().empty() ? "*" : clients[fd].getNickname();
		srv->sendReply(fd, prefix + " 431 " + currentNick + " :No nickname given\r\n");
		return ;
	}

	const std::string	&newNick = cmd.args[0];
	if (!isNickAvailable(srv, fd, newNick, prefix, clients))	return ;

	bool	wasRegistered = s_clients[fd].registered;
	std::string	oldPrefix = userPrefix(fd, clients);

	clients[fd].setNickname(newNick);
	std::string nickMsg = oldPrefix + " NICK :" + newNick + "\r\n";

	if (wasRegistered)
	{
		srv->sendReply(fd, nickMsg);
		for (std::map<std::string, ChannelInfo>::iterator it = s_channels.begin(); it != s_channels.end(); ++it)
		{
			if (isMember(it->first, fd))
				broadcastToChannelExcept(srv, it->first, fd, nickMsg);
		}
	}
	else if (!s_clients[fd].username.empty())
	{
		s_clients[fd].registered = true;
		sendWelcomeBurst(srv, fd, newNick, prefix);
	}
}

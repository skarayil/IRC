#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"

void	handleQuit(Server *srv, int fd, const Command &cmd,
			const std::map<int, Client> &clients)
{
	std::string	reason = cmd.args.empty() ? "Quit" : cmd.args[0];
	std::string	quitMsg = userPrefix(fd, clients)
		+ " QUIT :" + reason + "\r\n";

	for (std::map<std::string, ChannelInfo>::iterator it = s_channels.begin(); it != s_channels.end(); ++it)
	{
		if (isMember(it->first, fd))
			broadcastToChannelExcept(srv, it->first, fd, quitMsg);
	}
}

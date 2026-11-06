#include "Server.hpp"
#include "Command.hpp"
#include "Helpers.hpp"

void	handlePart(Server *srv, int fd, const Command &cmd,
			const std::map<int, Client> &clients)
{
	const std::string	prefix = ":ircserv";
	const std::string	&nick = clients.at(fd).getNickname();

	if (!requireRegistered(srv, fd, nick, prefix))				return ;
	if (!requireMinArgs(srv, fd, cmd, 1, "PART", nick, prefix))	return ;

	std::string	reason = (cmd.args.size() > 1) ? cmd.args[1] : nick;
	std::vector<std::string>	chans = splitByComma(cmd.args[0]);

	for (size_t i = 0; i < chans.size(); i++)
	{
		const std::string	&chan = chans[i];
		if (!requireChannelExists(srv, fd, chan, nick, prefix))	continue ;
		if (!requireMember(srv, fd, chan, nick, prefix))		continue ;

		std::string	partMsg = userPrefix(fd, clients) + " PART "
			+ chan + " :" + reason + "\r\n";
		broadcastToChannel(srv, chan, partMsg);
		leaveChannel(fd, chan);
	}
}

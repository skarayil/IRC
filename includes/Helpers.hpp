#ifndef COMMAND_HELPERS_HPP
# define COMMAND_HELPERS_HPP

# include "Server.hpp"
# include "Channel.hpp"
# include "Command.hpp"
# include <string>
# include <vector>
# include <map>

bool	requireRegistered(Server *srv, int fd, const std::string &nick,
						const std::string &prefix);
bool	requireAuthenticated(Server *srv, int fd, const std::string &prefix);
bool	requireNotRegistered(Server *srv, int fd, const std::string &prefix);
bool	requireMinArgs(Server *srv, int fd, const Command &cmd, size_t min,
					const std::string &cmdName, const std::string &nick,
					const std::string &prefix);
bool	requireChannelExists(Server *srv, int fd, const std::string &chan,
						const std::string &nick, const std::string &prefix);
bool	requireMember(Server *srv, int fd, const std::string &chan,
					const std::string &nick, const std::string &prefix);
bool	requireOp(Server *srv, int fd, const std::string &chan,
				const std::string &nick, const std::string &prefix);

void	broadcastToChannel(Server *srv, const std::string &chan,
						const std::string &msg);
void	broadcastToChannelExcept(Server *srv, const std::string &chan,
							int exceptFd, const std::string &msg);
void	removeMemberFromChannel(int fd, const std::string &chan);
void	removeOpFromChannel(int fd, const std::string &chan);
void	deleteChannelIfEmpty(const std::string &chan);
void	leaveChannel(int fd, const std::string &chan);

std::vector<std::string>	splitByComma(const std::string &input);
void	sendWelcomeBurst(Server *srv, int fd, const std::string &nick,
						const std::string &prefix);

#endif
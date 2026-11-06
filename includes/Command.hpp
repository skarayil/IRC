#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <vector>
# include <map>
# include "Client.hpp"

class Server;

struct Command
{
    std::string command;
    std::vector<std::string> args;
    std::string raw;
};

void handlePass(Server *srv, int fd, const Command &cmd, const std::string &serverPass);
void handleNick(Server *srv, int fd, const Command &cmd, std::map<int, Client> &clients);
void handleUser(Server *srv, int fd, const Command &cmd, const std::map<int, Client> &clients);

void handleJoin(Server *srv, int fd, const Command &cmd, const std::map<int, Client> &clients);
void handlePart(Server *srv, int fd, const Command &cmd, const std::map<int, Client> &clients);
void handleQuit(Server *srv, int fd, const Command &cmd, const std::map<int, Client> &clients);
void handlePrivmsg(Server *srv, int fd, const Command &cmd, const std::map<int, Client> &clients);

void handleKick(Server *srv, int fd, const Command &cmd, const std::map<int, Client> &clients);
void handleInvite(Server *srv, int fd, const Command &cmd, const std::map<int, Client> &clients);
void handleTopic(Server *srv, int fd, const Command &cmd, const std::map<int, Client> &clients);
void handleMode(Server *srv, int fd, const Command &cmd, const std::map<int, Client> &clients);

#endif

#include "Server.hpp"
#include "Channel.hpp"
#include <cctype>
#include "Command.hpp"

void Server::executeCommand(Client &client, const Command &cmd)
{
    int fd = client.getFd();
    if (s_clients.find(fd) == s_clients.end())
    {
        ClientInfo info;
        info.authenticated = false;
        info.registered    = false;
        info.username      = "";
        s_clients[fd] = info;
    }
    if (cmd.command.empty())
        return;

    std::string command = cmd.command;
    for (size_t i = 0; i < command.size(); i++)
        command[i] = std::toupper((unsigned char)command[i]);

    if      (command == "PASS")    handlePass(this, fd, cmd, _password);
    else if (command == "NICK")    handleNick(this, fd, cmd, _clients);
    else if (command == "USER")    handleUser(this, fd, cmd, _clients);
    else if (command == "JOIN")    handleJoin(this, fd, cmd, _clients);
    else if (command == "PART")    handlePart(this, fd, cmd, _clients);
    else if (command == "QUIT")    handleQuit(this, fd, cmd, _clients);
    else if (command == "PRIVMSG") handlePrivmsg(this, fd, cmd, _clients);
    else if (command == "KICK")    handleKick(this, fd, cmd, _clients);
    else if (command == "INVITE")  handleInvite(this, fd, cmd, _clients);
    else if (command == "TOPIC")   handleTopic(this, fd, cmd, _clients);
    else if (command == "MODE")    handleMode(this, fd, cmd, _clients);
    else if (command == "PING")
    {
        std::string token = cmd.args.empty() ? "" : cmd.args[0];
        sendReply(fd, ":ircserv PONG ircserv :" + token + "\r\n");
    }
}

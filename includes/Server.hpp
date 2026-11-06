#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Command.hpp"
# include "Parser.hpp"
# include "Bot.hpp"
# include <cstdlib>
# include <cstring>
# include <fcntl.h>
# include <iostream>
# include <map>
# include <netdb.h>
# include <netinet/in.h>
# include <poll.h>
# include <stdexcept>
# include <string>
# include <sys/socket.h>
# include <unistd.h>
# include <vector>

extern bool g_stop;

class Server
{
  private:
    int _port;
    std::string _password;
    int _server_fd;
    std::vector<struct pollfd> _pollfds;
    std::map<int, Client> _clients;

    void acceptClient();
    bool handleClientRead(int fd);
    bool handleClientWrite(int fd);
    void disconnectClient(int fd);
    struct pollfd *getPollFd(int fd);

  public:
    Server();
    Server(int port, const std::string &password);
    Server(const Server &other);
    Server &operator=(const Server &other);
    ~Server();

    void init();
    void run();
    void executeCommand(Client &client, const Command &cmd);
    void sendReply(int fd, const std::string &msg);
};

#endif

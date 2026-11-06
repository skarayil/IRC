#include "Server.hpp"
#include "Channel.hpp"
#include "Command.hpp"
#include "Parser.hpp"

Server::Server() : _port(0), _password(""), _server_fd(-1) {}

Server::Server(int port, const std::string &password) : _port(port), _password(password), _server_fd(-1) {}

Server::Server(const Server &other)
{
    *this = other;
}

Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        _port = other._port;
        _password = other._password;
        _server_fd = other._server_fd;
        _pollfds = other._pollfds;
        _clients = other._clients;
    }
    return *this;
}

Server::~Server()
{
    for (size_t i = 0; i < _pollfds.size(); i++)
    {
        if (_pollfds[i].fd != -1)
            close(_pollfds[i].fd);
    }
}

void Server::init()
{     
    struct addrinfo hints;
    struct addrinfo *res;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    std::stringstream ss;
    ss << _port;
    std::string port_str = ss.str();
    
    if (getaddrinfo(NULL, port_str.c_str(), &hints, &res) != 0)
    {
        throw std::runtime_error("Failed to get address info");
    }

    _server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (_server_fd == -1)
    {
        freeaddrinfo(res);
        throw std::runtime_error("Failed to create socket");
    }

    int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        freeaddrinfo(res);
        close(_server_fd);
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }

    if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        freeaddrinfo(res);
        close(_server_fd);
        throw std::runtime_error("Failed to set non-blocking flag");
    }

    if (bind(_server_fd, res->ai_addr, res->ai_addrlen) == -1)
    {
        freeaddrinfo(res);
        close(_server_fd);
        throw std::runtime_error("Failed to bind socket to port");
    }
    
    freeaddrinfo(res);

    if (listen(_server_fd, SOMAXCONN) == -1)
    {
        close(_server_fd);
        throw std::runtime_error("Failed to listen on socket");
    }
    std::cout << "Server is listening on port: " << _port << "..." << std::endl;

    struct pollfd server_pfd;
    server_pfd.fd = _server_fd;
    server_pfd.events = POLLIN;
    server_pfd.revents = 0;
    _pollfds.push_back(server_pfd);
}

void Server::disconnectClient(int fd)
{
    std::cout << "Istemci ayrildi! fd: " << fd << std::endl;
    cleanupZombieFD(fd);
    close(fd);
    _clients.erase(fd);
    for (size_t i = 0; i < _pollfds.size(); i++)
    {
        if (_pollfds[i].fd == fd)
        {
            _pollfds.erase(_pollfds.begin() + i);
            break;
        }
    }
}

struct pollfd *Server::getPollFd(int fd)
{
    for (size_t i = 0; i < _pollfds.size(); i++)
    {
        if (_pollfds[i].fd == fd)
            return &_pollfds[i];
    }
    return NULL;
}

void Server::acceptClient()
{
    int client_fd = accept(_server_fd, NULL, NULL);
    if (client_fd == -1)
        return;
    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    Client new_client(client_fd);
    _clients[client_fd] = new_client;

    struct pollfd client_pfd;
    client_pfd.fd = client_fd;
    client_pfd.events = POLLIN;
    client_pfd.revents = 0;
    _pollfds.push_back(client_pfd);

    std::cout << "Yeni istemci baglandi! fd: " << client_fd << std::endl;
}

bool Server::handleClientRead(int fd)
{
    char buf[1024];
    std::memset(buf, 0, sizeof(buf));

    int bytes = recv(fd, buf, sizeof(buf) - 1, 0);
    if (bytes <= 0)
    {
        disconnectClient(fd);
        return false;
    }

    _clients[fd].appendReadBuffer(std::string(buf, bytes));
    if (_clients[fd].getReadBuffer().size() > 4096)
    {
        disconnectClient(fd);
        return false;
    }

    std::string &client_buffer = _clients[fd].getReadBuffer();
    size_t pos;
    while ((pos = client_buffer.find("\n")) != std::string::npos)
    {
        std::string command_line = client_buffer.substr(0, pos);

        if (!command_line.empty() && command_line[command_line.length() - 1] == '\r')
        {
            command_line.erase(command_line.length() - 1);
        }

        _clients[fd].eraseReadBuffer(pos + 1);

        if (!command_line.empty())
        {
            Command cmd = Parser::parse(command_line);
            executeCommand(_clients[fd], cmd);
            std::string upper_cmd = cmd.command;
            for (size_t i = 0; i < upper_cmd.size(); i++)
                upper_cmd[i] = std::toupper((unsigned char)upper_cmd[i]);

            if (upper_cmd == "QUIT")
            {
                disconnectClient(fd);
                return false;
            }
        }
    }

    return true;
}

bool Server::handleClientWrite(int fd)
{
    int client_fd = fd;

    if (_clients.find(client_fd) != _clients.end())
    {
        std::string &wbuf = _clients[client_fd].getWriteBuffer();

        if (!wbuf.empty())
        {
            int sent = send(client_fd, wbuf.c_str(), wbuf.length(), 0);
            if (sent <= 0)
            {
                disconnectClient(client_fd);
                return false;
            }
            _clients[client_fd].eraseWriteBuffer(sent);
        }

        if (_clients[client_fd].getWriteBuffer().empty())
        {
            struct pollfd *pfd = getPollFd(client_fd);
            if (pfd)
                pfd->events &= ~POLLOUT;
        }
    }
    return true;
}

void Server::run()
{
    while (!g_stop)
    {
        int ret = poll(&_pollfds[0], _pollfds.size(), 1000);
        if (ret == -1)
        {
            if (g_stop)
                break;
            std::cerr << "Error! : poll";
            break;
        }
        for (size_t i = 0; i < _pollfds.size(); i++)
        {
            if (_pollfds[i].revents & POLLIN)
            {
                if (_pollfds[i].fd == _server_fd)
                {
                    acceptClient();
                }
                else
                {
                    if (!handleClientRead(_pollfds[i].fd))
                    {
                        i--;
                        continue;
                    }
                }      
            }
            if (i < _pollfds.size() && (_pollfds[i].revents & POLLOUT))
            {
                if (!handleClientWrite(_pollfds[i].fd))
                {
                    i--;
                    continue;
                }
            }
        }
        broadcastBotJoke(this);
    }
}

void Server::sendReply(int fd, const std::string &msg)
{
    if (_clients.find(fd) == _clients.end())
        return;

    _clients[fd].appendWriteBuffer(msg);

    struct pollfd *pfd = getPollFd(fd);
    if (pfd)
        pfd->events |= POLLOUT;
}

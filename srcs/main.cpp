#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cctype>
#include "Server.hpp"

bool g_stop = false;

void signalHandler(int signum)
{
    (void)signum;
    std::cout << "\nSignal received! Shutting down" << std::endl;
    g_stop = true;
}

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    std::string port_str = av[1];
    if (port_str.empty() || port_str.length() > 5)
    {
        std::cerr << "Error: Wrong port parameters" << std::endl;
        return 1;
    }

    for (size_t i = 0; i < port_str.length(); i++)
    {
        if (!isdigit(port_str[i]))
        {
            std::cerr << "Error: Port must contain only digits" << std::endl;
            return 1;
        }
    }

    int port = std::atoi(av[1]);
    if (port < 1024 || port > 65535)
    {
        std::cerr << "Error: Wrong port parameters" << std::endl;
        return 1;
    }

    std::string password = av[2];
    if (password.empty())
    {
        std::cerr << "Error: Password cannot be empty!" << std::endl;
        return 1;
    }
    
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGPIPE, SIG_IGN);

    try
    {
        Server server(port, password);
        server.init();
        server.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    std::cout << "Server closed cleanly." << std::endl;
    return 0;
}

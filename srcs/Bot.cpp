#include "Bot.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Helpers.hpp"
#include <ctime>
#include <iostream>

void broadcastBotJoke(Server *srv)
{
    static time_t lastJokeTime = std::time(NULL);
    time_t now = std::time(NULL);

    if (now - lastJokeTime < 300)
        return;

    lastJokeTime = now;
    if (s_channels.empty())
        return;

    std::string joke = getRandomJoke();
    std::cout << "[JokeBot] " << joke << std::endl;

    for (std::map<std::string, ChannelInfo>::iterator it = s_channels.begin(); it != s_channels.end(); ++it)
    {
        const std::string &chan = it->first;
        std::string msg = ":JokeBot!bot@localhost PRIVMSG " + chan + " :" + joke + "\r\n";
        broadcastToChannel(srv, chan, msg);
    }
}

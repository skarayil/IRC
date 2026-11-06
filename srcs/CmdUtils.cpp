#include "Server.hpp"
#include "Channel.hpp"
#include "Command.hpp"
#include <algorithm>
#include <sstream>

std::map<int, ClientInfo>          s_clients;
std::map<std::string, ChannelInfo> s_channels;

bool channelExists(const std::string &name)
{
    return s_channels.find(name) != s_channels.end();
}

bool isOp(const std::string &channel, int fd)
{
    if (!channelExists(channel))
        return false;
    const std::vector<int> &ops = s_channels[channel].operators;
    return std::find(ops.begin(), ops.end(), fd) != ops.end();
}

bool isMember(const std::string &channel, int fd)
{
    if (!channelExists(channel))
        return false;
    const std::vector<int> &members = s_channels[channel].members;
    return std::find(members.begin(), members.end(), fd) != members.end();
}

int findFdByNick(const std::map<int, Client> &clients, const std::string &nick)
{
    for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getNickname() == nick)
            return it->first;
    }
    return -1;
}

std::string userPrefix(int fd, const std::map<int, Client> &clients)
{
    std::string nick = "unknown";
    std::string user = "unknown";
    if (clients.find(fd) != clients.end())
        nick = clients.at(fd).getNickname();
    if (s_clients.find(fd) != s_clients.end())
        user = s_clients.at(fd).username;
    if (user.empty())
        user = nick;
    return ":" + nick + "!" + user + "@localhost";
}

void cleanupZombieFD(int fd)
{
    s_clients.erase(fd);
    std::map<std::string, ChannelInfo>::iterator it = s_channels.begin();
    while (it != s_channels.end())
    {
        ChannelInfo &chan = it->second;

        std::vector<int>::iterator mIt = std::find(chan.members.begin(), chan.members.end(), fd);
        if (mIt != chan.members.end())
            chan.members.erase(mIt);

        std::vector<int>::iterator opIt = std::find(chan.operators.begin(), chan.operators.end(), fd);
        if (opIt != chan.operators.end())
            chan.operators.erase(opIt);

        std::vector<int>::iterator invIt = std::find(chan.invited.begin(), chan.invited.end(), fd);
        if (invIt != chan.invited.end())
            chan.invited.erase(invIt);

        if (chan.members.empty())
        {
            std::map<std::string, ChannelInfo>::iterator toErase = it;
            ++it;
            s_channels.erase(toErase);
        }
        else
        {
            ++it;
        }
    }
}

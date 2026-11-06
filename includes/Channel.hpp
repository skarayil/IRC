#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include <map>
# include "Client.hpp"

struct ClientInfo
{
    std::string username;
    bool        authenticated;
    bool        registered;
};

struct ChannelInfo
{
    std::vector<int>         members;
    std::vector<int>         operators;
    std::vector<int>         invited;
    std::string              topic;
    std::string              key;
    int                      userLimit;
    bool                     inviteOnly;
    bool                     topicOps;
};

extern std::map<int, ClientInfo>          s_clients;
extern std::map<std::string, ChannelInfo> s_channels;

bool        channelExists(const std::string &name);
bool        isOp(const std::string &channel, int fd);
bool        isMember(const std::string &channel, int fd);
int         findFdByNick(const std::map<int, Client> &clients, const std::string &nick);
std::string userPrefix(int fd, const std::map<int, Client> &clients);
void        cleanupZombieFD(int fd);

#endif
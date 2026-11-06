#include "Parser.hpp"
#include "Command.hpp"

Command Parser::parse(const std::string &raw_line)
{
    Command cmd;

    cmd.raw = raw_line;
    std::stringstream ss(raw_line);
    std::string token;

    if (!(ss >> token))
    {
        return cmd;
    }
    cmd.command = token;
    while (ss >> token)
    {
        if (token[0] == ':')
        {
            std::string trailing = token.substr(1);
            std::string rest;
            std::getline(ss, rest);
            cmd.args.push_back(trailing + rest);
            break;
        }
        else
            cmd.args.push_back(token);
    }

    return cmd;
}
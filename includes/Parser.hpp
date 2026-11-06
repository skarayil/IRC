#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "Command.hpp"

class Parser
{
    private:
    Parser();
    ~Parser();
    Parser(const Parser &other);
    Parser &operator=(const Parser &other);
        
    public:
        static Command parse(const std::string &raw_line);
};



#endif
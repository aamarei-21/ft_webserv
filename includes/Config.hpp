#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Parser.hpp"
#include "Server.hpp"

using std::string;

class Config {
public:

    void parse_config(int ac, char **av)
    {
        char str[] = "./default";
	    Parser conf((ac >= 2 ? av[1] : str));

        conf.getContent(servers);        
    }

    std::vector<Server> servers;
};


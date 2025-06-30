#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <csignal>
#include "include/ServerConf.hpp"
#include "include/Parser.hpp"
#include "include/CheckConfig.hpp"
#include "include/Tokenizer.hpp"

bool g_signal = false;

void signalHandler(int signum) 
{
    if (signum == SIGINT || signum == SIGTERM)
        g_signal = true;
}

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " <config_file>" << std::endl;
		return 1;
	}
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, SIG_IGN);
	
	CheckConfig checkConf(av[1]);
	checkConf.checkConfig();
	return 0;
}
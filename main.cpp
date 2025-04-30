#include <iostream>
#include "include/ServerConf.hpp"
#include "include/Parser.hpp"
#include "include/CheckConfig.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " <config_file>" << std::endl;
		return 1;
	}
	CheckConfig checkConf(av[1]);
	checkConf.checkConfig();
	return 0;
}
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
# include "ServerConf.hpp"
class Parser {
	protected:
		std::string confKey[18];
		std::vector<ServerConf> serverConfVec;
	public:
		Parser();
		~Parser();
		Parser(const Parser &other);
		Parser &operator=(const Parser &other);


	
		void initKey();
		std::map<std::string, int> confKeyMap(std::string);
		void checkKeyCount(std::map<std::string, int>);
};
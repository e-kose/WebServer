#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <fstream>

class Parser {
	public:
		Parser();
		~Parser();
		Parser(const Parser &other);
		Parser &operator=(const Parser &other);

		std::vector<std::string> split(const std::string &str, char delimiter);
		std::vector<std::string> parseConfigFile(const std::string &filename);
		void parseLine(const std::string &line, std::map<std::string, std::string> &configMap);
		void printConfig(const std::map<std::string, std::string> &configMap);
};
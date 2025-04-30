#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <fstream>

class Parser {
	private:
		std::string	filename;
		std::ifstream	file;
		std::string	line;
		
	public:
		Parser();
		~Parser();
		Parser(const Parser &other);
		Parser &operator=(const Parser &other);

};
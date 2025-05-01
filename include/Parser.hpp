#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>

class Parser {
	protected:
		std::string fileName;
		std::ifstream confFile;
		std::string resConf;
		std::string confKey[18];
	public:
		Parser(std::string fileName);
		~Parser();
		Parser(const Parser &other);
		Parser &operator=(const Parser &other);

		const std::string& getFileName() ;
		 std::ifstream& getConfFile() ;
		 std::string& getResConf() ;

		void setFileName(const std::string &fileName);
		void setConfFile(const std::ifstream &confFile);
		void setResConf(const std::string &resConf);
		
	
		std::vector<std::string> split(const std::string &str, char delimiter);
		std::vector<std::string> parseConfigFile(const std::string &filename);
		void parseLine(const std::string &line, std::map<std::string, std::string> &configMap);
		void printConfig(const std::map<std::string, std::string> &configMap);
};
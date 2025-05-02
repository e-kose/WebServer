#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
# include "ServerConf.hpp"
class Parser {
	protected:
		std::string fileName;
		std::ifstream confFile;
		std::string resConf;
		std::string confKey[18];
		std::vector<ServerConf> serverConfVec;
	public:
		Parser();
		Parser(std::string fileName);
		~Parser();
		Parser(const Parser &other);
		Parser &operator=(const Parser &other);


		void printServerConfVec(std::vector<ServerConf>& serverConfVec); //BOS METOD

		const std::string& getFileName() ;
		std::ifstream& getConfFile() ;
		std::string& getResConf() ;

		void setFileName(const std::string &fileName);
		void setConfFile(const std::ifstream &confFile);
		void setResConf(const std::string &resConf);
		
	
		void initKey();
		std::map<std::string, int> confKeyMap(std::string);
		void checkKeyCount(std::map<std::string, int>);
};
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:42:10 by menasy            #+#    #+#             */
/*   Updated: 2025/05/01 20:40:22 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/CheckConfig.hpp"
#include <unistd.h>
CheckConfig::CheckConfig(std::string argv) : Parser(argv) {
	inServer = false;
}
CheckConfig::~CheckConfig() {
}
CheckConfig::CheckConfig(const CheckConfig &other) : Parser(other) {
}
CheckConfig &CheckConfig::operator=(const CheckConfig &other) {
	if (this != &other) {
		Parser::operator=(other);
	}
	return *this;
}


static std::string trimLine(const std::string& str) {
    size_t start = 0;
    while (start < str.length() && std::isspace(str[start]))
        ++start;

    size_t end = str.length();
    while (end > start && std::isspace(str[end - 1]))
        --end;

    return str.substr(start, end - start);
}

void CheckConfig::checkFileExtensions() 
{
	std::string ext = this->fileName.substr(this->fileName.find_last_of(".") + 1);
	if (ext != "conf" )
		throw std::runtime_error("File extension is not .conf");
}
static size_t characterCounter(const std::string& str, char c)
{
	size_t counter = 0;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == c)
			counter++;
	}
	return counter;
}
static bool isJustCharacter(const std::string& str, char c)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] != c)
		{
			if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
				continue;
			else
				return false;
		}
	}
	return true;
}

void CheckConfig::checkConfKey(const std::string& element)
{
	for (size_t i = 0; i < sizeof(this->confKey) / sizeof(std::string); i++)
	{	
		if (element == this->confKey[i])
			return;
	}
	throw std::runtime_error("Unknown directive KEYYYYY");
}
static bool	semiColonCheck(const std::string& str)
{
	if (str.find_first_of(";") == std::string::npos)
	{
		if (isJustCharacter(str, '{') || isJustCharacter(str, '}') || str.find("location") != std::string::npos)
			return true;
		else
			return false;
	}
	if (str.find_first_of(";") != std::string::npos)
	{
		if (isJustCharacter(str,';') || str.find_first_of(";") != str.length() -1)
			return false;
		else
			return true;
	}
	return true;
}
void CheckConfig::checkElements(std::string str)
{
	std::string::size_type pos;
	std::string element, line, tmp = str;
	size_t index = 1;
	if (str.find_first_of("{") != std::string::npos && str[0] != '{')
	{
		tmp = trimLine(str.substr(0,str.find_first_of("{")));
		tmp = tmp.substr(0, tmp.find_first_of(" \t\n\0"));
		if (tmp != "server")
			throw std::runtime_error("Unknown directive");
	}
	pos = str.find_first_of("{");
	if (pos != std::string::npos && str[pos +1] && str[pos + 1] == '\n')
		index = 2;
	tmp = str.substr(str.find_first_of("{") + index, str.length());
	while (true)
	{
		pos = tmp.find_first_of('\n');
		if (pos == std::string::npos)
			break;
		line = trimLine(tmp.substr(0, pos + 1));
		// std::cout << "==================LINE=================== \n" << line << std::endl;
		if (!semiColonCheck(line))
			throw std::runtime_error("Missing semicolon");
		element = line.substr(0,line.find_first_of(" \t\n\0"));
		// std::cout << "==================ELEMENT=================== \n" << element << std::endl;
		checkConfKey(element);
		tmp = tmp.substr(pos +1, tmp.length());
	}
}

void CheckConfig::bracketsCheck(std::string str)
{
	std::string::size_type pos1, pos2;
	std::string sub, serverStr, tmp;
	size_t index = 0;
	if (characterCounter(str, '{') != characterCounter(str, '}'))
		throw std::runtime_error("Unexpected end of file, expecting paranthesis");
	while (true)
	{
		if (!this->inServer)
		{
			tmp = str;
			pos1 = tmp.find_first_of("{");
			if (pos1 == std::string::npos)
				break;
			this->inServer = true;
		}
		pos2 = tmp.find_first_of("}");
		index += pos2 + 1;
		sub = str.substr(pos1,  index - pos1);
		if (characterCounter(sub,'{') == characterCounter(sub,'}'))
		{
			serverStr = str.substr(0, index);
			// std::cout << "==================SERVER STR=================== \n" << serverStr << std::endl;
			checkElements(serverStr);
			str = str.substr(serverStr.length(), str.length());
			this->inServer = false;
			index = 0;
		}
		else
			tmp = str.substr(index,str.length());
	}
}

std::string CheckConfig::fileHandler() 
{
	this->confFile.open(this->fileName.c_str());
	if (!this->confFile.is_open())
		throw std::runtime_error("No such file or directory");
	std::string line, trimedLine, destStr;
	while (std::getline(this->confFile, line))
	{
		trimedLine = trimLine(line);
		if (trimedLine.empty() || trimedLine[0] == '#')	
			continue;
		destStr += trimedLine + "\n";
	}
	return destStr;
}
void CheckConfig::checkConfig() {
	
	std::string tmpRes;
	try
	{
		checkFileExtensions();
		tmpRes = fileHandler();
		bracketsCheck(tmpRes);
		this->resConf = tmpRes;
		Tokenizer tokenizer(this->resConf);
		std::vector<std::string> tek = tokenizer.seperation();
		std::vector<ServerConf> conf = tokenizer.createConfVec(tek);
		// std::cout << "==================CONFIGURATION FILE=================== \n" << this->resConf << std::endl;

		std::cout << "==================SERVER CONFIGURATION=================== \n";
		std::vector<ServerConf>::iterator it = conf.begin();
	for (; it != conf.end() ; it++)
	{
		
		std::cout << "Server IP: " << it->getIp() << std::endl;
		std::cout << "Server Port: " << it->getPort() << std::endl;
		std::cout << "Server Root: " << it->getRoot() << std::endl;
		std::cout << "Server Name: ";
		std::vector<std::string> servername = it->getServerName();
		std::vector<std::string>::iterator it2 = servername.begin();
		for (; it2 != servername.end() ; it2++)
			std::cout << *it2 << " ";
		std::cout << std::endl;
		std::cout << "Server Index: ";
		std::vector<std::string> index = it->getIndex();
		std::vector<std::string>::iterator it3 = index.begin();
		for (; it3 != index.end() ; it3++)
			std::cout << *it3 << " ";
		std::cout << std::endl;
		std::cout << "Server Body Size: " << it->getBodySize() << std::endl;
		std::cout << "Server Error Log: " << it->getErrorLog() << std::endl;
		std::cout << "Server Access Log: " << it->getAccesLog() << std::endl;
		std::cout << "Server Error Pages: ";
		std::map<int, std::string> erorpages = it->getErrorPages();
		std::map<int, std::string>::iterator end = erorpages.end();
		std::map<int, std::string>::iterator it4 = erorpages.begin();
		for (; it4 != end; it4++)
			std::cout << it4->first << " " << it4->second << " ";
		std::cout << std::endl;
		std::cout << "Server Locations: ";
		std::vector<LocationConf> locations = it->getLocations();
		int i = 0;
		for (; i < locations.size() ; i++)
		{
			std::cout << "Location Path: " << locations[i].getPath() << std::endl;
			std::cout << "Location AutoIndex: " << locations[i].getAutoIndex() << std::endl;
			std::cout << "Location Root: " << locations[i].getRoot() << std::endl;
			std::cout << "Location Cgi Extension: " << locations[i].getCgiExtension() << std::endl;
			std::cout << "Location Cgi Path: " << locations[i].getCgiPath() << std::endl;
			std::cout << "Location Cgi Pass: " << locations[i].getCgiPass() << std::endl;
			std::cout << "Location Upload Store: " << locations[i].getUploadStore() << std::endl;
			std::cout << "Location Methods: ";
			std::vector<std::string> methods = locations[i].getMethods();
			std::vector<std::string>::iterator it6 = methods.begin();
			for (; it6 != methods.end() ; it6++)
				std::cout << *it6 << " ";
			std::cout << std::endl;
			std::cout << "Location Try Files: ";
			std::vector<std::string> tryfiles = locations[i].getTryFiles();
			std::vector<std::string>::iterator it7 = tryfiles.begin();
			for (; it7 != tryfiles.end() ; it7++)
				std::cout << *it7 << " ";
			std::cout << std::endl;
			std::cout << "Location Index: ";
			std::vector<std::string> index = locations[i].getIndex();
			std::vector<std::string>::iterator it8 = index.begin();
			for (; it8 != index.end() ; it8++)
				std::cout << *it8 << " ";
			std::cout << std::endl;
			std::map<int, std::string> retur = locations[i].getReturn();
			std::map<int ,std::string>::iterator it9 = retur.begin();
			for (; it9 != retur.end() ; it9++)
				std::cout << it9->first << " " << it9->second << " ";
			std::cout << std::endl;
			std::cout << "----------------------------------------" << std::endl;
		}
	}
		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
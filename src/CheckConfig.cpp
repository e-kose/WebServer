/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:42:10 by menasy            #+#    #+#             */
/*   Updated: 2025/04/30 21:02:40 by menasy           ###   ########.fr       */
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

void CheckConfig::checkConfKey(const std::string& element)
{
	for (size_t i = 0; i < sizeof(this->confKey) / sizeof(std::string); i++)
	{	
		if (element == this->confKey[i])
			return;
	}
	throw std::runtime_error("Unknown directive KEYYYYY");
}
void CheckConfig::checkElements(std::string str)
{

	std::string tmp = str;
	std::string element, line;
	if (str.find_first_of("{") != std::string::npos && str[0] != '{')
	{
		tmp = trimLine(str.substr(0,str.find_first_of("{")));
		tmp = tmp.substr(0, tmp.find_first_of(" \t\n\0"));
		if (tmp != "server")
			throw std::runtime_error("Unknown directive");
	}
	tmp = str.substr(str.find_first_of("{") + 1, str.length());
	std::string::size_type pos1;
	std::cout << "TMP: \n" << tmp;
	while (true)
	{
		pos1 = tmp.find_first_of(';');
		if (pos1 == std::string::npos)
			break;
		line = trimLine(tmp.substr(0, pos1 + 1));
		std::cout << "LINE:" << line;
		element = line.substr(0,line.find_first_of(" \t\n\0"));
		std::cout << "ELEMENT:" << element;
		checkConfKey(element);
		tmp = tmp.substr(pos1 +1, tmp.length());
		std::cout << "TMP2:" << tmp<< std::endl;
	}
}

void CheckConfig::bracketsCheck(std::string& str)
{

	if (characterCounter(str, '{') != characterCounter(str, '}'))
		throw std::runtime_error("Unexpected end of file, expecting paranthesis");
	std::string::size_type pos1, pos2;
	std::string sub, serverStr, tmp;
	size_t index = 0;
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
		sub = str.substr(pos1,  index);
		if (characterCounter(sub,'{') == characterCounter(sub,'}'))
		{
			serverStr = str.substr(0, index + pos1);
			std::cout << "SERVER STR: \n" << serverStr ;
			checkElements(serverStr);
			str = str.substr(serverStr.length(), str.length());
			this->inServer = false;
			index = 0;
		}
		else
		{
			tmp = str.substr(index,str.length());
		}
	}
	std::cout << "==========================" << std::endl;
}
// void	CheckConfig::semiColonCheck(const std::string& str)
// {
// 	std::string::size_type posBrackets = str.find_first_of("{");
// 	this->inServer = true;
// 	while (this->inServer)
// 	{
// 		std::string::size_type posSemiColon = str.find_first_of(";");
// 	}
// }
std::string CheckConfig::fileHandler() 
{
	this->confFile.open(this->fileName.c_str());
	if (!this->confFile.is_open())
		throw std::runtime_error("No such file or directory");
	std::string line;
	std::string trimedLine;
	std::string destStr;
	while (std::getline(this->confFile, line))
	{
		trimedLine = trimLine(line);
		if (trimedLine.empty() || trimedLine[0] == '#')	
			continue;
		destStr += trimedLine + "\n";
	}
	bracketsCheck(destStr);
	return destStr;
}
void CheckConfig::checkConfig() {
	
	try
	{
		checkFileExtensions();
		fileHandler();
		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}
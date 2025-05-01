/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:42:10 by menasy            #+#    #+#             */
/*   Updated: 2025/05/01 14:23:37 by menasy           ###   ########.fr       */
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
			std::cout << "==================SERVER STR=================== \n" << serverStr << std::endl;
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
	
	try
	{
		checkFileExtensions();
		bracketsCheck(fileHandler());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	std::cout << "============ SUCCESLY FINISHED ==============" << std::endl;
}
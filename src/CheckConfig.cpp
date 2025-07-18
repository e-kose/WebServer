/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:42:10 by menasy            #+#    #+#             */
/*   Updated: 2025/07/04 09:00:40 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/CheckConfig.hpp"
#include "../include/ServerConf.hpp"

CheckConfig::CheckConfig():Tokenizer(){}
CheckConfig::CheckConfig(std::string argv) {
	this->fileName = argv;
	inServer = false;
}
CheckConfig::~CheckConfig() {
}
CheckConfig::CheckConfig(const CheckConfig &other) : Tokenizer(other) {
}
CheckConfig &CheckConfig::operator=(const CheckConfig &other) {
	if (this != &other) {
		Tokenizer::operator=(other);
	}
	return *this;
}

void CheckConfig::checkFileExtensions() 
{
	std::string ext = this->fileName.substr(this->fileName.find_last_of(".") + 1);
	if (ext != "conf" )
		throw std::runtime_error("File extension is not .conf");
}

void CheckConfig::checkConfKey(const std::string& element)
{
	for (size_t i = 0; i < sizeof(this->confKey) / sizeof(std::string); i++)
	{	
		if (element == this->confKey[i])
			return;
	}
	throw std::runtime_error("Failed directive: " + element);
}

void CheckConfig::checkElements(std::string str)
{
	std::string::size_type pos;
	std::string element, line, tmp = str;
	size_t index = 1;
	if (str.find("server") != std::string::npos)
	{
		if (str.find_first_of("{") != std::string::npos)
		{
			tmp = HelperClass::trimLine(str.substr(0, str.find_first_of("{")));
			tmp = tmp.substr(0, tmp.find_first_of(" \t\n\0"));
		}
		else 
			tmp = HelperClass::trimLine(str.substr(0,str.find_first_of(" \n\t")));	
		if (tmp != "server")
			throw std::runtime_error("Missing server directive");
	}
	else
		throw std::runtime_error("Missing server directive!");

	pos = str.find_first_of("{");
	if (pos != std::string::npos && str[pos +1] && str[pos + 1] == '\n')
		index = 2;
	tmp = str.substr(str.find_first_of("{") + index, str.length());
	while (true)
	{
		pos = tmp.find_first_of('\n');
		if (pos == std::string::npos)
			break;
		line = HelperClass::trimLine(tmp.substr(0, pos + 1));
		if (!HelperClass::semiColonCheck(line))
			throw std::runtime_error("Missing semicolon: " + line);
		element = line.substr(0,line.find_first_of(" \t\n\0"));
		checkConfKey(element);
		tmp = tmp.substr(pos +1, tmp.length());
	}
}

void CheckConfig::bracketsCheck(std::string str)
{
	std::string::size_type pos1, pos2;
	std::string sub, serverStr, tmp;
	size_t index = 0;
	if (HelperClass::characterCounter(str, '{') != HelperClass::characterCounter(str, '}'))
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
		if (HelperClass::characterCounter(sub,'{') == HelperClass::characterCounter(sub,'}'))
		{
			serverStr = str.substr(0, index);
			checkElements(serverStr);
			str = str.substr(serverStr.length(), str.length());
			this->inServer = false;
			index = 0;
		}
		else
			tmp = str.substr(index,str.length());
	}
}
void CheckConfig::checkRequiredElements()
{
	if (this->serverConfVec.empty())
		throw std::runtime_error("No server directive found");
	std::vector<ServerConf>::iterator it = this->serverConfVec.begin();
	for (; it != this->serverConfVec.end(); it++)
	{
		if (it->getLocations().empty())
			throw std::runtime_error("No location directive found in server block");
		if (it->getRoot().empty())
			throw std::runtime_error("No root directive found in server block");
		if (it->getServerName().empty())
			throw std::runtime_error("No server_name directive found in server block");
	}
}

std::string CheckConfig::fileHandler() 
{
	std::ifstream confFile;
	confFile.open(this->fileName.c_str());
	if (!confFile.is_open())
		throw std::runtime_error("No such file or directory");
	std::string line, trimedLine, destStr;
	while (std::getline(confFile, line))
	{
		trimedLine = HelperClass::trimLine(line);
		if (trimedLine.empty() || trimedLine[0] == '#')	
			continue;
		else if (trimedLine.find("#") != std::string::npos)
			trimedLine = trimedLine.substr(0, trimedLine.find("#"));
		trimedLine += '\n';
		destStr.append(trimedLine);
	}
	if (destStr.empty() || HelperClass::isJustCharacter(destStr, '\n') 
		|| HelperClass::isJustCharacter(destStr, '\t') || HelperClass::isJustCharacter(destStr, ' ')) 
		throw std::runtime_error("Empty file");
	confFile.close();
	return destStr;
}

void CheckConfig::checkConfig() {
	
	try
	{
		checkFileExtensions();
		this->fullText = fileHandler();
		bracketsCheck(this->fullText);
		std::vector<std::string> tek = this->seperation();
		this->serverConfVec =  this->createConfVec(tek);
		this->checkRequiredElements();
		HelperClass::fillScriptExtVec();
	 	HelperClass::fillFileExtVec();		
		WebServer web(serverConfVec);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
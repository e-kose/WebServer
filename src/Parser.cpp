/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:39:27 by menasy            #+#    #+#             */
/*   Updated: 2025/07/04 08:53:47 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Parser.hpp"

Parser::Parser(){initKey();}

Parser::~Parser() {}
Parser::Parser(const Parser &other)
{
	*this = other;
}
Parser &Parser::operator=(const Parser &other) {
	if (this == &other) {
		return (*this);
	}
	this->serverConfVec = other.serverConfVec;
	return *this;
}

void Parser::initKey()
{
	this->confKey[0]  = "autoindex";
	this->confKey[1]  = "methods";
	this->confKey[2]  = "try_files";
	this->confKey[3]  = "upload_dir";
	this->confKey[4]  = "return";
	this->confKey[5]  = "cgi_pass";
	this->confKey[6]  = "cgi_ext";
	this->confKey[7]  = "root";
	this->confKey[8]  = "index";	
	this->confKey[9] = "listen";
	this->confKey[10] = "server_name";
	this->confKey[11] = "client_max_body_size";
	this->confKey[12] = "access_log";
	this->confKey[13] = "error_log";
	this->confKey[14] = "error_page";
	this->confKey[15]  = "location";
	this->confKey[16] = "{";
	this->confKey[17] = "}";
}

std::map<std::string, int> Parser::confKeyMap(std::string sectionKey)
{
	std::map<std::string, int> confKeyMap;
	if (sectionKey == "location"){
		for (int i = 0; i < 8; i++)
			confKeyMap.insert(std::pair<std::string, int>(this->confKey[i], 0));
	}
	else{
		for (int i = 13; i < 8; i++)
			confKeyMap.insert(std::pair<std::string, int>(this->confKey[i], 0));
	}
	return confKeyMap;
}

void Parser::checkKeyCount(std::map<std::string, int> confKeyCounter)
{
	std::map<std::string, int>::iterator it = confKeyCounter.begin();
	for (; it != confKeyCounter.end(); it++)
		if (it->second > 1)
			throw std::runtime_error("Duplicate key found");
}

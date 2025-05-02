/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:39:27 by menasy            #+#    #+#             */
/*   Updated: 2025/05/03 00:21:58 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Parser.hpp"

Parser::Parser(){initKey();}

Parser::~Parser() {}
Parser::Parser(const Parser &other){}
Parser &Parser::operator=(const Parser &other) {
	return *this;
}

void Parser::initKey()
{
	this->confKey[0]  = "autoindex";     // location
	this->confKey[1]  = "methods";       // location
	this->confKey[2]  = "try_files";     // location
	this->confKey[3]  = "upload_dir";    // location
	this->confKey[4]  = "return";        // location
	this->confKey[5]  = "cgi_pass";      // location
	this->confKey[6]  = "cgi_ext";       // location
	this->confKey[7]  = "root";          // hem server hem location
	this->confKey[8]  = "index";         // hem server hem location
	
	this->confKey[9] = "listen";        // server
	this->confKey[10] = "server_name";   // server
	this->confKey[11] = "client_max_body_size"; // server
	this->confKey[12] = "access_log";    // server
	this->confKey[13] = "error_log";     // server
	this->confKey[14] = "error_page";    // server
	this->confKey[15]  = "location";      // location başlangıcı
	this->confKey[16] = "{";             // blok başlangıcı
	this->confKey[17] = "}";             // blok sonu
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

void Parser::printServerConfVec(std::vector<ServerConf>& serverConfVec) //BOS METOD
{
	std::cout << "==================SERVER CONF VEC=================== \n";
	for (size_t i = 0; i < serverConfVec.size(); i++)
	{
		std::cout << "********* " << i + 1 <<". "<< "Server *******" << std::endl;
		std::cout << "\tIP: " << serverConfVec[i].getIp() << std::endl;
		std::cout << "\tPort: " << serverConfVec[i].getPort() << std::endl;
	}
}
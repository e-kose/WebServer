/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:39:27 by menasy            #+#    #+#             */
/*   Updated: 2025/05/01 15:32:22 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Parser.hpp"

Parser::Parser(std::string fileName) : fileName(fileName) 
{
	this->confKey[0] = "listen";
	this->confKey[1] = "server_name";
	this->confKey[2] = "client_max_body_size";
	this->confKey[3] = "error_page"; 
	this->confKey[4] = "root";
	this->confKey[5] = "index"; 
	this->confKey[6] = "access_log";
	this->confKey[7] = "error_log";
	this->confKey[8] = "location"; 
	this->confKey[9] = "autoindex";
	this->confKey[10] = "methods";
	this->confKey[11] = "try_files";
	this->confKey[12] = "upload_dir";
	this->confKey[13] = "return";
	this->confKey[14] = "cgi_pass";
	this->confKey[15] = "cgi_ext";
	this->confKey[16] = "{";
	this->confKey[17] = "}";
	
	
	
}
Parser::~Parser() {}
Parser::Parser(const Parser &other) : fileName(other.fileName) {}
Parser &Parser::operator=(const Parser &other) {
	if (this != &other) {
		fileName = other.fileName;
	}
	return *this;
}

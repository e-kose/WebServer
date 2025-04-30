/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 12:39:27 by menasy            #+#    #+#             */
/*   Updated: 2025/04/30 21:01:23 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Parser.hpp"

Parser::Parser(std::string fileName) : fileName(fileName) 
{
	this->confKey[0] = "listen";
    this->confKey[1] = "server_name";
    this->confKey[2] = "root";
    this->confKey[3] = "index";
    this->confKey[4] = "client_max_body_size";
    this->confKey[5] = "error_page";
    this->confKey[6] = "location";
    this->confKey[7] = "methods";
    this->confKey[8] = "autoindex";
    this->confKey[9] = "upload_store";
    this->confKey[10] = "{";
    this->confKey[11] = "}";
	
	
}
Parser::~Parser() {}
Parser::Parser(const Parser &other) : fileName(other.fileName) {}
Parser &Parser::operator=(const Parser &other) {
	if (this != &other) {
		fileName = other.fileName;
	}
	return *this;
}

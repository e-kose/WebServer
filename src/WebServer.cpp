/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/05/02 16:26:48 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

WebServer::WebServer() {}
WebServer::WebServer(std::vector<ServerConf>& serverConfVec) : serverConfVec(serverConfVec) {
}
WebServer::WebServer(const WebServer &other) : serverConfVec(other.serverConfVec) {
}
WebServer &WebServer::operator=(const WebServer &other) {
	if (this != &other) {
		this->serverConfVec = other.serverConfVec;
	}
	return *this;
}
WebServer::~WebServer() {
}

bool WebServer::isExistIpAndPort(const std::string& ip, int& port)
{
	std::map<int,ServerConf>::iterator it;
	it = this->socketMap.begin();
	while(it++ != this->socketMap.end())
		if (it->second.getIp() == ip && it->second.getPort() == port)
			true;
	return false;
}
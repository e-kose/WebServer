/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/05/02 18:05:05 by ekose            ###   ########.fr       */
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
	for (std::map<int, ServerConf>::iterator it = socketMap.begin(); it != socketMap.end(); ++it)
		close(it->first);
}

void WebServer::initSocket()
{
	for (int i = 0; i < this->serverConfVec.size(); i++){
		sockaddr_in hint;
		int sockfd;
		int opt = 1;
		int inetPtnStatus;
		ServerConf& conf = serverConfVec[i];
		
		if (isExistIpAndPort(conf.getIp(), conf.getPort())){
			std::cerr << "Duplicate IP and Port in configuration: " << conf.getIp() << ":" << conf.getPort() << std::endl;
			continue;
		}
		if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
			throw std::runtime_error("Socket Not Create");
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("Setsockopt failed");
		
		hint.sin_family = AF_INET;
		hint.sin_port = htons(conf.getPort());
		inetPtnStatus = inet_pton(AF_INET, conf.getIp().c_str(), &hint.sin_addr);
		if (inetPtnStatus== 0)
			throw std::runtime_error("Invalid IP Address: " + conf.getIp());
		else if (inetPtnStatus == -1)
			throw std::runtime_error("Inet_pton Failed");
		if (bind(sockfd, (sockaddr*)&hint, sizeof(hint)) == -1){
			if (errno == EADDRINUSE)
				std::cerr << "Port in use: " << conf.getIp() << ":" << conf.getPort() << std::endl;
			else
				perror("Bind");
			close(sockfd);
			continue;
		}
		if (listen(sockfd, SOMAXCONN) == -1)
			throw std::runtime_error("Listen failed on socket for " + conf.getIp() + ":" + std::to_string(conf.getPort()));
		this->socketMap[sockfd] = (this->serverConfVec[i]);
	}
}

bool WebServer::isExistIpAndPort(const std::string& ip, int port)
{
	std::map<int,ServerConf>::iterator it;
	it = this->socketMap.begin();
	while(it++ != this->socketMap.end())
		if (it->second.getIp() == ip && it->second.getPort() == port)
			return true;
	return false;
}

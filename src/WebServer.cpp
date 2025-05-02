/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/05/02 17:06:29 by ekose            ###   ########.fr       */
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

void WebServer::initSocket(){
		for (int i = 0; i < this->serverConfVec.size(); i++){
			sockaddr_in hint;
			int sockfd;
			int opt = 1;
			int inetPtnStatus;
			ServerConf& conf = serverConfVec[i];
			
			if (isExistIpAndPort(conf.getIp(), conf.getPort()))
				throw std::runtime_error("Ip and Port Already");
			if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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
			if (bind(sockfd, (sockaddr*)&hint, sizeof(hint)) == -1)
				throw std::runtime_error("Binding Failed");
			if (listen(sockfd, SOMAXCONN) == -1)
				throw std::runtime_error("Listen failed on socket for " + conf.getIp() + ":" + std::to_string(conf.getPort()));
			this->socketMap[sockfd] = (this->serverConfVec[i]);
		}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/05/03 16:49:27 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

WebServer::WebServer() 
{
	initSocket();
	runServer();
}

WebServer::WebServer(std::vector<ServerConf>& serverConfVec) : serverConfVec(serverConfVec) 
{
	initSocket();
	runServer();
}

WebServer::WebServer(const WebServer &other) : serverConfVec(other.serverConfVec) {}

WebServer &WebServer::operator=(const WebServer &other)
 {
	if (this != &other) {
		this->serverConfVec = other.serverConfVec;
	}
	return *this;
}

WebServer::~WebServer()
{
	for (std::map<int, ServerConf>::iterator it = socketMap.begin(); it != socketMap.end(); ++it)
	close(it->first);
}

void	WebServer::setNonBlocking(int fd)
{
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);	
}

bool	WebServer::isExistIpAndPort(const std::string& ip, int port)
{
	std::map<int,ServerConf>::iterator it;
	it = this->socketMap.begin();
	while(it != this->socketMap.end()){
		if (it->second.getIp() == ip && it->second.getPort() == port)
			return true;
		it++;
	}
	return false;
}

void	WebServer::initSocket()
{
	for (int i = 0; i < this->serverConfVec.size(); i++){
		sockaddr_in hint;
		int sockfd;
		int opt = 1;
		int inetPtnStatus;
		ServerConf& conf = serverConfVec[i];
		
		if (isExistIpAndPort(conf.getIp(), conf.getPort()))
		{
			std::cerr << "Duplicate IP and Port in configuration: " << conf.getIp() << ":" << conf.getPort() << std::endl;
			continue;
		}
		if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
			throw std::runtime_error("Socket Not Create");
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("Setsockopt failed");
		this->setNonBlocking(sockfd);
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
	std::cout << "------> " << serverConfVec.size()<< std::endl;
}


void	WebServer::runServer()
{
	std::vector<pollfd> pollVec;
	std::map<int, ServerConf>::iterator socketIt = this->socketMap.begin();
	
	for (; socketIt != this->socketMap.end(); socketIt++)
	{
		pollfd fd = {socketIt->first, (POLLIN | POLLOUT | POLLERR), 0};
		pollVec.push_back(fd);
	}

	while (true)
	{
		int result = poll(pollVec.data(), pollVec.size(), 0);
		if (result < 0)
			throw std::runtime_error("poll() error. Terminating server.");
		
		for (int i = 0; i < pollVec.size(); i++)
		{
			if (pollVec[i].revents & POLLIN)
			{
				// Yeni client mi geldi kontrol etmek için
				ServerConf* match = NULL;
				for (socketIt = socketMap.begin(); socketIt != socketMap.end(); socketIt++)
				{
					if (socketIt->first == pollVec[i].fd)
					{
						match = &socketIt->second; 
						break;
					}
				}
				// Yeni Clientten veri Geldi İse
				if (match)
				{
					int	clientSock = accept(pollVec[i].fd, NULL, NULL);
					if (clientSock < 0)
						throw std::runtime_error("Socket Not Create");
					this->setNonBlocking(clientSock);
					pollfd clientFd = {clientSock, (POLLIN | POLLOUT | POLLERR), 0};
					pollVec.push_back(clientFd);
					this->clientToServerMap[clientFd.fd] = match;
					//////////////////////|| ACCES LOG EKLENEBİLİR||////////////////////////
					std::cout << "New connection arrived " << clientSock << std::endl;
				}
				// Mevcut clientten veri geldi ise
				else
				{
					std::cout << this->clientToServerMap[pollVec[i].fd]->getIndex()[0]<<std::endl;
					char buffer[1024] = {};
					int readByte = recv(pollVec[i].fd, buffer, sizeof(buffer), 0);
					if (readByte < 0)
					{
						std::cout << "Client ayrıldı\n";
						close(pollVec[i].fd);
						pollVec.erase(pollVec.begin() + i);
						i--;
					}
					else
						std::cout << buffer << std::endl;
				}
			}
		}
	}
	
}
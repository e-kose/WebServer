/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/05/08 20:37:58 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

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
	for (std::vector<pollfd>::iterator it = pollVec.begin(); it != pollVec.end(); ++it)
		close(it->fd);
}

void	WebServer::setNonBlocking(int fd)
{
	// BAKALIM BUNA
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void 	WebServer::closeCliSocket(int fd)
{
	if (fd >= 0)
	{
		for (std::vector<pollfd>::iterator it = pollVec.begin(); it != pollVec.end(); it++)
		{
			if (it->fd == fd)
			{
				pollVec.erase(it); 
				break;
			}
		}
	}
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

void	WebServer::pollfdVecCreat()
{
	std::map<int, ServerConf>::iterator socketIt = this->socketMap.begin();
	
	for (; socketIt != this->socketMap.end(); socketIt++)
	{
		pollfd fd = {socketIt->first, (POLLIN | POLLOUT | POLLERR), 0};
		pollVec.push_back(fd);
	}	
}

std::string WebServer::socketInfo(int sockfd)
{
	std::ostringstream oss;
	sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	if (getpeername(sockfd, (sockaddr*)&addr, &addrlen) == -1)
	{
		oss << "Error getting socket info: " << strerror(errno);
		close(sockfd);
		return "";
	}
	oss << "Socket info: " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port);
	return oss.str();
}

void	WebServer::initSocket()
{
	for (int i = 0; i < this->serverConfVec.size(); i++)
	{
		sockaddr_in hint;
		int sockfd;
		int opt = 1;
		int inetPtnStatus;
		ServerConf& conf = serverConfVec[i];
		std::ostringstream oss;
		oss << conf.getPort();
		
		if (isExistIpAndPort(conf.getIp(), conf.getPort()))
		{
			HelperClass::writeToFile(conf.getErrorLog(), "Duplicate IP and Port in configuration: " + conf.getIp() + ":" + oss.str());
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
				HelperClass::writeToFile(conf.getErrorLog(), "Port in use: " + conf.getIp()+ ":" + oss.str());
			else
				HelperClass::writeToFile(conf.getErrorLog(), "Bind error: " + std::string(strerror(errno)));
			close(sockfd);
			continue;
		}
		if (listen(sockfd, SOMAXCONN) == -1)
			throw std::runtime_error("Listen failed on socket for " + conf.getIp() + ":" + oss.str());
		this->socketMap[sockfd] = (this->serverConfVec[i]);
	}
}

HttpRequest*	WebServer::pollInEvent(pollfd& pollStruct)
{
	HttpRequest* httpRequest = NULL;
	std::map<int, ServerConf>::iterator socketIt;
	// Yeni client mi geldi kontrol etmek için
	ServerConf* match = NULL;
	for (socketIt = socketMap.begin(); socketIt != socketMap.end(); socketIt++)
	{
		if (socketIt->first == pollStruct.fd)
		{
			match = &socketIt->second; 
			break;
		}
	}
	// Yeni Clientten veri Geldi İse
	if (match)
	{
		int	clientSock = accept(pollStruct.fd, NULL, NULL);
		if (clientSock < 0)
		{
			HelperClass::writeToFile(match->getErrorLog(), "Connection Refused");
			return NULL;
		}
		this->setNonBlocking(clientSock);
		pollfd clientFd = {clientSock, (POLLIN | POLLOUT | POLLERR), 0};
		pollVec.push_back(clientFd);
		this->clientToServerMap[clientFd.fd] = match;
		HelperClass::writeToFile(match->getAccessLog(), "New connection from client: " + socketInfo(clientSock));
	}
	// Mevcut clientten veri geldi ise
	else
	{
		char buffer[1024] = {};
		int readByte = recv(pollStruct.fd, buffer, sizeof(buffer), 0);
		if (readByte <= 0)
		{
			std::cout << "Client ayrıldı\n";
			this->closeCliSocket(pollStruct.fd);
		}
		else
		{
			httpRequest = this->parseRecv(std::string(buffer));
		}
	}
	return httpRequest;
}

// void	WebServer::pollOutEvent(pollfd& pollStruct)
// {
	
// }

HttpRequest* WebServer::parseRecv(const std::string& request)
{
	HttpRequest* httpRequest = new HttpRequest();
	httpRequest->parseRequest(request);
	return httpRequest;
	
}

void	WebServer::runServer()
{
	HttpRequest* httpRequest = NULL;
	this->pollfdVecCreat();
	while (true)
	{
		int result = poll(pollVec.data(), pollVec.size(), -1);
		if (result < 0)
			throw std::runtime_error("poll() error. Terminating server.");
		for (int i = 0; i < pollVec.size(); i++)
		{
			if (pollVec[i].revents & POLLIN)
				httpRequest = this->pollInEvent(pollVec[i]);
			// std::cout << "BOOOODDDDY\n" << httpRequest->getBody() << std::endl;
			// if (pollVec[i].revents & POLLOUT)
			// {
			// 	std::cout << "POLL OUT EVENT" << std::endl;
			// 	// Handle POLLOUT event here
			// }
			
		}
	}
	
}
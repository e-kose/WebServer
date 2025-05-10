/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/05/10 17:34:48 by ekose            ###   ########.fr       */
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
	for (std::map<int, std::vector<ServerConf>>::iterator it = socketMap.begin(); it != socketMap.end(); ++it)
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

bool	WebServer::isExistIpAndPort(ServerConf& conf)
{
	std::map<int,std::vector<ServerConf>>::iterator it;
	it = this->socketMap.begin();
	while(it != this->socketMap.end()){
		if (it->second[0].getIp() == conf.getIp() && it->second[0].getPort() == conf.getPort())
		{
			it->second.push_back(conf);
			return true;
		}
		it++;
	}
	return false;
}

void	WebServer::pollfdVecCreat()
{
	std::map<int, std::vector<ServerConf>>::iterator socketIt = this->socketMap.begin();
	
	for (; socketIt != this->socketMap.end(); socketIt++)
	{
		pollfd fd = {socketIt->first, (POLLIN | POLLOUT | POLLERR), 0};
		pollVec.push_back(fd);
	}	
}

std::string WebServer::socketInfo(sockaddr_in& addr, int clientSock)
{
	std::ostringstream oss;
	sockaddr_in hint;
	socklen_t addrlen = sizeof(hint);
	oss << "Socket info: " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port);
	if (getsockname(clientSock, (sockaddr*)&hint, &addrlen) == -1)
	{
		oss << "Error getting socket name: " << strerror(errno);
		return oss.str();
	}
	oss << " -> " << inet_ntoa(hint.sin_addr) << ":" << ntohs(hint.sin_port);
	return oss.str();
}

ServerConf& WebServer::searchServerConf(std::vector<ServerConf>& confVec, std::string serverName)
{
	for (size_t i = 0; i < confVec.size(); i++)
	{
		std::vector<std::string> serverNames = confVec[i].getServerName();
		for (size_t j = 0; j < serverNames.size(); j++)
		{
			if (serverNames[j] == serverName)
				return confVec[i];
		}
	}
	return confVec[0];
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
		
		if (isExistIpAndPort(conf))
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
		this->socketMap[sockfd].push_back(this->serverConfVec[i]);
	}
}

void	WebServer::newClient(pollfd& pollStruct)
{	
	sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int	clientSock = accept(pollStruct.fd, (sockaddr*)&addr, &addrlen);
	if (clientSock < 0)
	{
		if (socketMap.count(pollStruct.fd))
			for (int j = 0; j < socketMap[pollStruct.fd].size(); j++)
				HelperClass::writeToFile(socketMap[pollStruct.fd][j].getErrorLog(), "Accept error: " + std::string(strerror(errno)));
		return;
	}
	this->setNonBlocking(clientSock);
	pollfd clientFd = {clientSock, (POLLIN | POLLOUT | POLLERR), 0};
	pollVec.push_back(clientFd);
	
}
void	WebServer::clientRead(pollfd& pollStruct)
{
	char buffer[1024] = {};
	int readByte = recv(pollStruct.fd, buffer, sizeof(buffer), 0);
	if (readByte < 0)
	{
		std::cout << "Client ayrıldı\n";
		this->closeCliSocket(pollStruct.fd);
	}
	else
	{
		this->clientToHttpRequestMap[pollStruct.fd] =  this->parseRecv(std::string(buffer));
		this->clientToServerMap[pollStruct.fd] = &this->searchServerConf(serverConfVec, this->clientToHttpRequestMap[pollStruct.fd]->getHostName());
		std::cout << "fd : " << pollStruct.fd << std::endl;
		pollStruct.events = POLLOUT;
	}
}

HttpRequest* WebServer::parseRecv(const std::string& request)
{
	HttpRequest* httpRequest = new HttpRequest();
	httpRequest->parseRequest(request);
	return httpRequest;
	
}
void WebServer::pollOutEvent(pollfd& pollStruct, HttpRequest* httpRequest)
{
	if (httpRequest == NULL)
	{
		std::cout << "HttpRequest is NULL" << std::endl;
		pollStruct.events = POLLIN;
		return;
	}
	std::cout << "POLL_OUT_GET METHOD:" << httpRequest->getMethod() << std::endl;
	if (httpRequest->getMethod() == "GET")
	{
		std::string response = "HTTP/1.1 200 OK\r\n";
		response += "Content-Type: text/html\r\n";
		response += "Content-Length: 13\r\n";
		response += "Connection: keep-alive\r\n";
		response += "\r\n";
		response += "<h1>Hello</h1>";
		send(pollStruct.fd, response.c_str(), response.size(), 0);
		pollStruct.events = POLLIN;
		httpRequest = NULL;
	}
	// else if (httpRequest->getMethod() == "POST")
	// {
	// 	std::cout << "POST METHOD" << std::endl;
	// }
	// else if (httpRequest->getMethod() == "DELETE")
	// {
	// 	std::cout << "DELETE METHOD" << std::endl;
	// }
	// else
	// {
	// 	std::cout << "UNKNOWN METHOD" << std::endl;
	// }
}
void	WebServer::runServer()
{
	this->pollfdVecCreat();
	while (true)
	{
		int result = poll(pollVec.data(), pollVec.size(), -1);
		if (result < 0)
			throw std::runtime_error("poll() error. Terminating server.");
		for (int i = 0; i < pollVec.size(); i++)
		{
			//Yeni client geldi ise
			if (socketMap.count(pollVec[i].fd) && (pollVec[i].revents & POLLIN))
			{
				this->newClient(pollVec[i]);
				pollVec[i].events = POLLOUT;
				continue;
			}
			//Client'dan veri geldi ise
			if (pollVec[i].revents & POLLIN)
			{
				this->clientRead(pollVec[i]);
				pollVec[i].events = POLLOUT;
				continue;
			}
			if (pollVec[i].revents & POLLOUT)
			{
				std::cout << "POLL_OUT" << std::endl;
				// HttpRequest* httpRequest = this->parseRecv("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
				this->pollOutEvent(pollVec[i], this->clientToHttpRequestMap[pollVec[i].fd]);
				pollVec[i].events = POLLIN;
				continue;
				// delete httpRequest;
			}
		}
	}
	
}
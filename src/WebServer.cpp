/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/05/11 13:12:41 by menasy           ###   ########.fr       */
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
	for (std::map<int, std::vector<ServerConf> >::iterator it = socketMap.begin(); it != socketMap.end(); ++it)
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
	std::map<int,std::vector<ServerConf> >::iterator it;
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
	std::map<int, std::vector<ServerConf> >::iterator socketIt = this->socketMap.begin();
	
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
HttpRequest* WebServer::parseRecv(const std::string& request)
{
	std:: cout << "================== REQUEST ================== \n"  << request << std::endl;
	HttpRequest* httpRequest = new HttpRequest();
	httpRequest->parseRequest(request);
	return httpRequest;
	
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

HttpRequest*	WebServer::pollInEvent(pollfd& pollStruct)
{
	int check = 0;
	HttpRequest* httpRequest = NULL;
	std::map<int, std::vector<ServerConf> >::iterator socketIt;
	// Yeni client mi geldi kontrol etmek için
	for (socketIt = socketMap.begin(); socketIt != socketMap.end(); socketIt++)
	{
		// Yeni Clientten veri Geldi İse
		if (socketIt->first == pollStruct.fd)
		{
			sockaddr_in addr;
			socklen_t addrlen = sizeof(addr);
			int	clientSock = accept(pollStruct.fd, (sockaddr*)&addr, &addrlen);
			if (clientSock < 0)
			{
				for (int j = 0; j < socketIt->second.size(); j++)
				{
					if (socketIt->second[j].getPort() == ntohs(addr.sin_port))
						HelperClass::writeToFile(socketIt->second[j].getErrorLog(), "Accept error: " + std::string(strerror(errno)));
				}
				return NULL;
			}
			this->setNonBlocking(clientSock);
			pollfd clientFd = {clientSock, (POLLIN | POLLOUT | POLLERR), 0};
			pollVec.push_back(clientFd);
			check = 1;
			break;
		}
	}
	// Mevcut clientten veri geldi ise
	if (check == 0)
	{
		char buffer[1024] = {};
		int readByte = recv(pollStruct.fd, buffer, sizeof(buffer), 0);
		if (readByte <= 0)
		{
			std::cout << "!!!!!!!!!! Client ayrıldı !!!!!!!!!!!!!!\n";
			this->closeCliSocket(pollStruct.fd);
		}
		else
		{
			httpRequest = this->parseRecv(std::string(buffer));
			this->clientToServerMap[pollStruct.fd] = &this->searchServerConf(serverConfVec, httpRequest->getHostName());
			// std::cout << "PollIn_fd : " << pollStruct.fd << std::endl;
		}
	}
	return httpRequest;
}

void WebServer::tryFiles(const LocationConf& locConf, const std::string& httpPath, const std::string& rootPath)
{
	if (!locConf.getRoot().empty())
	{
		//hehrangi bir dizinde bulunmazasa bunu yapacağım. 
	}
}

std::string WebServer::findRequest(HttpRequest* httpRequest, pollfd& pollStruct)
{
	std::cout << "GET METHOD HANDLER" << std::endl;
	ServerConf* serverConfMap = this->clientToServerMap[pollStruct.fd];
	std::vector<LocationConf> locVec = serverConfMap->getLocations();
	std::string httpPath , mergedPath;	
	httpPath = httpRequest->getPath();
	for (size_t i = 0; i < locVec.size(); i++)
	{
		if (httpPath == locVec[i].getPath())
		{
			if (locVec[i].getRoot().empty())
				mergedPath = HelperClass::mergeDirectory(serverConfMap->getRoot(), httpPath);
			else
				mergedPath = HelperClass::mergeDirectory(locVec[i].getRoot(), httpPath);
			if (!httpRequest->getRequestFile().empty())
				mergedPath += "/" + httpRequest->getRequestFile();
			break ;
		}
	}
	std::cout << "MERGED PATH: " << mergedPath << std::endl;
	return mergedPath;
}

void WebServer::pollOutEvent(pollfd& pollStruct, HttpRequest* httpRequest)
{
	if (httpRequest == NULL)
	{
		std::cout << "HttpRequest is NULL" << std::endl;
		return;
	}
	findRequest(httpRequest, pollStruct);
	if (httpRequest->getMethod() == "GET")
	{
		std::cout << "GET METHOD" << std::endl;
		
	}
	else if (httpRequest->getMethod() == "POST")
	{
		std::cout << "POST METHOD" << std::endl;
	}
	else if (httpRequest->getMethod() == "DELETE")
	{
		std::cout << "DELETE METHOD" << std::endl;
	}
	else
	{
		std::cout << "UNKNOWN METHOD" << std::endl;
	}
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
			{
				// std::cout << "POLLFD: " << pollVec[i].fd << std::endl;
				httpRequest = this->pollInEvent(pollVec[i]); // delete yapmayı unutma 
				if (httpRequest)
				{
				
					pollVec[i].events = POLLOUT;
				}
			}
			if (pollVec[i].revents & POLLOUT)
			{
				std::cout << "POLL OUT EVENT" << std::endl;
				this->pollOutEvent(pollVec[i], httpRequest);	
				
				pollVec[i].events = POLLIN;
			}
			
		}
	}
	
}
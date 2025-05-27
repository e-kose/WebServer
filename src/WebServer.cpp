/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/05/27 23:36:21 by menasy           ###   ########.fr       */
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
		this->socketMap = other.socketMap;
		this->pollVec = other.pollVec;
		this->clientToServerMap = other.clientToServerMap;
		this->clientToAddrMap = other.clientToAddrMap;
		this->resultPath = other.resultPath;
		
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
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void 	WebServer::closeCliSocket(int fd)
{
	std::cout << ">>>>>>>>>>>>> Closing client socket: " << fd <<" <<<<<<<<<<<<<<"<< std::endl;
	if (fd >= 0)
	{
		for (std::vector<pollfd>::iterator it = pollVec.begin(); it != pollVec.end(); it++)
		{
			if (it->fd == fd)
			{
				if (this->clientRequests[fd])
					delete this->clientRequests[fd];
				this->clientRequests[fd] = NULL;
				close(fd);
				clientToServerMap.erase(fd);
				pollVec.erase(it);
				clientToAddrMap.erase(fd);
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
	for (size_t i = 0; i < this->serverConfVec.size(); i++)
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
void	WebServer::acceptNewClient(pollfd& pollStruct)
{
	std::map<int, std::vector<ServerConf> >::iterator socketIt = this->socketMap.begin();
	while (true)
	{
		sockaddr_in addr;
		socklen_t addrlen = sizeof(addr);
		int	clientSock = accept(pollStruct.fd, (sockaddr*)&addr, &addrlen);
		if (clientSock < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break ;
			for (size_t j = 0; j < socketIt->second.size(); j++)
			{
				if (socketIt->second[j].getPort() == ntohs(addr.sin_port))
					HelperClass::writeToFile(socketIt->second[j].getErrorLog(), "Accept error: " + std::string(strerror(errno)));
			}
			return ;
		}
		this->setNonBlocking(clientSock);
		pollfd clientFd = {clientSock, (POLLIN | POLLOUT | POLLERR), 0};
		pollVec.push_back(clientFd);
		this->clientToAddrMap[clientSock] = addr;
	}
}

void WebServer::clientRead(pollfd& pollStruct)
{
	//// FONKSİYON GÜNCELLENECEK HEADERDAN SONRA BODYYİ ALACAK
    char buffer[4096];
    std::string requestData;
    bool keepReading = true;

    while (keepReading)
    {
        int bytesReceived = recv(pollStruct.fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived < 0)
        {
			// SUBJECTE UYGUN DEĞİL GBİ BAKICAM
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
            {
                std::cerr << ">>>> Error reading from client: " << strerror(errno) << " <<<<" <<std::endl;
                closeCliSocket(pollStruct.fd);
                return;
            }
        }
        else if (bytesReceived == 0)
        {
            std::cout << ">>>> Client Ayrıldı <<<<" << std::endl;
            closeCliSocket(pollStruct.fd);
            return;
        }
        else
        {
            buffer[bytesReceived] = '\0';
            requestData += buffer;

            if (requestData.find("\r\n\r\n") != std::string::npos)
            {
                keepReading = false;
            }
        }
    }

    if (!requestData.empty())
    {
        try
        {
            if (this->clientRequests[pollStruct.fd])
			{
                delete this->clientRequests[pollStruct.fd];
				this->clientRequests[pollStruct.fd] = NULL;
			}
            this->clientRequests[pollStruct.fd] = this->parseRecv(requestData);
            this->clientToServerMap[pollStruct.fd] = &this->searchServerConf(this->serverConfVec, this->clientRequests[pollStruct.fd]->getHostName());
			this->clientRequests[pollStruct.fd]->sepPath(this->clientToServerMap[pollStruct.fd]->getLocations());
			pollStruct.events = POLLOUT;
		}
        catch(const std::exception& e)
        {
            std::cerr << "Error parsing request: " << e.what() << std::endl;
            closeCliSocket(pollStruct.fd);
            return;
        }
    }
}
void WebServer::sendHandler(pollfd& pollStruct, std::string& sendMessage)
{
	int checkSend = 0;
	int lengthMessage;
	
	lengthMessage = sendMessage.length();
	while (checkSend < lengthMessage)
	{
		checkSend = send(pollStruct.fd, sendMessage.c_str(), sendMessage.length(), 0);
		if (checkSend == -1)
		{
			int& errCode = errno;
			if (errCode == EAGAIN || errCode == EWOULDBLOCK)
			{
				//Soket non-blocking modda ve veri göndermek için yeterli buffer alanı yok demek. Yeniden veri gondercem.
				pollStruct.events = POLLOUT;
				break ; 
			}
			else
			{
				//errCode == ECONNRESET ||  errCode == EPIPE || errCode == EBADF
				//İstemci bağlantıyı aniden kapatmış.
				if (errCode != EBADF)
					close(pollStruct.fd);
				for (std::vector<pollfd>::iterator it = this->pollVec.begin(); it != this->pollVec.end(); ++it) 
				{
					if (it->fd == pollStruct.fd) 
					{
						this->pollVec.erase(it);
						break; 
					}
				}
				break ;
			}
		}
	}
}

bool WebServer::methodIsExist(const std::vector<std::string>& locMethodsvec, const std::string& requestMethod,pollfd& pollStruct)
{
	bool check = false;
	for (size_t i = 0; i < locMethodsvec.size(); i++)
	{
		if (locMethodsvec[i] == requestMethod)
			check = true;
	}
	if (!check)
	{
		std::string sendMessage;
		this->sendResponse(pollStruct, "405 Method Not Allowed");
		return false;
	}
	return true;
}
void WebServer::tryFiles(LocationConf& locConf, const std::string& httpPath, const ServerConf* serverConf,  pollfd& pollStruct)
{
	std::cout << "--------------- TRYFİLES --------------: "<< httpPath << std::endl;
	std::vector<std::string> tryFilesVec = locConf.getTryFiles();
	std::string newRoot, contentFile, errPage, sendMessage, resultDirectory;
	std::string::size_type pos1;
	newRoot = locConf.getRoot();
	if (newRoot.empty())
		newRoot = serverConf->getRoot();
	for (size_t i = 0; i < tryFilesVec.size(); i++)
	{
		pos1 = tryFilesVec[i].find("$uri");
		if (pos1 == std::string::npos && HelperClass::strIsDigit(tryFilesVec[i]))
			break;
		if (pos1 + 4 == tryFilesVec[i].length())
			tryFilesVec[i] = httpPath;
		else
			tryFilesVec[i] = httpPath + tryFilesVec[i].substr(pos1 + 4, tryFilesVec[i].length());
		resultDirectory = HelperClass::mergeDirectory(newRoot, tryFilesVec[i]);
		contentFile = this->readHtmlFile(pollStruct,resultDirectory, *serverConf);
		if (contentFile == "Forbidden")
			break;
		if (!contentFile.empty() || HelperClass::fileIsExist(resultDirectory))
		{
			std::vector<std::string> indexVec;
			indexVec = locConf.getIndex();
			if (indexVec.size() == 0)
				indexVec = serverConf->getIndex();
			if (HelperClass::indexHandler(resultDirectory, indexVec))
			{
				this->resultPath = resultDirectory;
				return ;
			}
			else if (locConf.getAutoIndex())
			{
				// dizindeki dosyalar listelenecek etonun metodla
				// etoya sade gidcek gelen istek
				return ;
			}
			else
			{
				this->resultPath = "";
				this->sendResponse(pollStruct,"403 Forbidden");
				return ;
			}		
		}
	}
	if (contentFile.empty() || !HelperClass::fileIsExist(resultDirectory))
	{
		errPage = tryFilesVec[tryFilesVec.size() -1];
		this->sendResponse(pollStruct, errPage + " Not Found");
	}
	std::cout << "------------------ TRY FİLES FİNİSHED -----------------" << std::endl;
}


std::string WebServer::findRequest(pollfd& pollStruct)
{
	std::cout << "------------ FIND REQUEST ----------" << std::endl;
	ServerConf* serverConf = this->clientToServerMap[pollStruct.fd];
	std::vector<LocationConf> locVec = serverConf->getLocations();
	std::string httpPath , mergedPath, tmpPath;
	size_t rootIndex = 0;		
	httpPath = this->clientRequests[pollStruct.fd]->getPath();
	if (!this->clientRequests[pollStruct.fd]->getRequestFile().empty())
		tmpPath = (httpPath + this->clientRequests[pollStruct.fd]->getRequestFile());
	std::cout << ">>>> HTTP PATH: " << httpPath << " <<<<"<< std::endl;
	for (size_t i = 0; i < locVec.size(); i++)
	{
		if (locVec[i].getPath() == "/")
			rootIndex = i;
		if (httpPath == locVec[i].getPath())
		{
			if (methodIsExist(locVec[i].getMethods(),this->clientRequests[pollStruct.fd]->getMethod(), pollStruct))
			{
				mergedPath = HelperClass::mergePath(*serverConf, locVec[i],
						this->clientRequests[pollStruct.fd]->getRequestFile(), httpPath);
				break;
			}
			else
				return "";
		}
	}
	if (mergedPath.empty() || HelperClass::fileIsExist(mergedPath))
	{
		std::cout << "go try: " << tmpPath << std::endl;
		tryFiles(locVec[rootIndex], tmpPath, serverConf, pollStruct);
		return this->resultPath;
	}
	else if(!HelperClass::fileIsExist(mergedPath) && readHtmlFile(pollStruct,mergedPath,*serverConf).empty())
	{
		std::cout << "Merged Path Is Not Exists: " << mergedPath << std::endl;
		this->sendResponse(pollStruct,"404 Not Found");
		return "";
	}
	std::cout << "-------- MERGED PATH: " << mergedPath << " --------------"<<std::endl;
	return mergedPath;
}

void WebServer::pollOutEvent(pollfd& pollStruct)
{
	if (this->clientRequests[pollStruct.fd] == NULL)
	{
		std::cout << ">>>> ClientRequest["<< pollStruct.fd << "] = NULL <<<"<< std::endl;
		return;
	}
	// std::string method = this->clientRequests[pollStruct.fd]->getMethod();
	this->resultPath = findRequest(pollStruct);
	if (this->resultPath.empty())
	{
		std::cout << ">>>> Result path is empty<<<" << std::endl;
		return ;
	}
	if (this->clientRequests[pollStruct.fd]->getMethod() == "GET")
	{
		std::cout << "------------ GET METHOD ---------" << std::endl;
		this->sendResponse(pollStruct, "200 OK");
	}
	else if (this->clientRequests[pollStruct.fd]->getMethod() == "POST")
	{
		
	}
	else if (this->clientRequests[pollStruct.fd]->getMethod() == "DELETE")
	{
		std::cout << "---------- DELETE METHOD HANDLER -----------" << std::endl;
		deleteMethod(pollStruct);
	}
	else
	{
		std::cout << "----------- UNKNOWN METHOD ---------" << std::endl;
	}
}
void	WebServer::runServer()
{
	this->pollfdVecCreat();
	while (true)
	{
		int result = poll(pollVec.data(), pollVec.size(), -1);
		if (result < 0)
			throw std::runtime_error("poll() error. Terminating server.");
		for (size_t i = 0; i < pollVec.size(); i++)
		{
			 if (pollVec[i].revents & POLLERR) 
			{
        		HelperClass::writeToFile(this->clientToServerMap[pollVec[i].fd]->getErrorLog(), "POLL ERROR");
        		closeCliSocket(pollVec[i].fd);
        		continue;
    		}
			if (this->socketMap.count(pollVec[i].fd) && (pollVec[i].revents & POLLIN))
			{
				this->acceptNewClient(pollVec[i]);
				continue;
			}

			if (pollVec[i].revents & POLLIN)
			{
				this->clientRead(pollVec[i]);
				pollVec[i].events = POLLOUT;
				continue;
			}
			
			if (pollVec[i].revents & POLLOUT)
			{
				std::cout << "---------------- POLL OUT EVENT ------------" << std::endl;
				this->pollOutEvent(pollVec[i]);	
				pollVec[i].events = POLLIN;
			}

		}

	}
}
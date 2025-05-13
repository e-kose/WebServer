/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/05/13 18:11:03 by menasy           ###   ########.fr       */
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
                std::cerr << "Error reading from client: " << strerror(errno) << std::endl;
                closeCliSocket(pollStruct.fd);
                return;
            }
        }
        else if (bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
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

void WebServer::tryFiles(LocationConf& locConf, const std::string& httpPath, const ServerConf* serverConfMap,  pollfd& pollStruct)
{
	std::vector<std::string> tryFilesVec = locConf.getTryFiles();
	std::string newRoot, contentFile, errPage, sendMessage, resultDirectory;
	std::string::size_type pos1;
	newRoot = locConf.getRoot();
	if (newRoot.empty())
		newRoot = serverConfMap->getRoot();
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
		contentFile = HelperClass::readHtmlFile(resultDirectory);
		if (!contentFile.empty())
		{	
			sendMessage = HelperClass::createHttpResponse("200","OK", "text/html", contentFile);
			sendHandler(pollStruct,sendMessage);
			return ;
		}
	}
	if (contentFile.empty())
	{
		errPage = tryFilesVec[tryFilesVec.size() -1];
		sendMessage = HelperClass::createErrorResponse(errPage + "Not Found", *serverConfMap, newRoot);
		this->sendHandler(pollStruct,sendMessage);
	}
	
}

bool WebServer::methodIsExist(const std::vector<std::string>& locMethodsvec, const std::string& requestMethod, ServerConf* srvConf, pollfd& pollStruct)
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
		sendMessage = HelperClass::createErrorResponse("405 Method Not Allowed", *srvConf, srvConf->getRoot());	
		sendHandler(pollStruct,sendMessage);
		return false;
	}
	return true;
}
void WebServer::sendHandler(pollfd& pollStruct, std::string& sendMessage)
{
	// 3 kere sayfayı yenileyince seg yiyoruz bakcam.
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
bool WebServer::indexHandler(pollfd& pollStruct, std::string& mergedPath, const std::vector<std::string>& indexVec )
{
	std::string tmpPath = mergedPath;
	for (size_t j = 0; j < indexVec.size(); j++)
	{
		tmpPath += indexVec[j];
		if (HelperClass::fileIsExist(tmpPath))
		{
			mergedPath = tmpPath;
			return true;
		}
		tmpPath = mergedPath;
	}
	this->sendResponse(pollStruct,"404 Not Found");
	return false;
}

std::string WebServer::findRequest(pollfd& pollStruct)
{
	std::cout << "FIND REQUEST" << std::endl;
	ServerConf* serverConfMap = this->clientToServerMap[pollStruct.fd];
	std::vector<LocationConf> locVec = serverConfMap->getLocations();
	std::cout << "SERVER ?????????????????????????\n";
	std::string httpPath , mergedPath;
	size_t rootIndex = 0;		
	httpPath = this->clientRequests[pollStruct.fd]->getPath();
	std::cout << "HTTP PATH: " << httpPath << std::endl;
	for (size_t i = 0; i < locVec.size(); i++)
	{
		if (locVec[i].getPath() == "/")
			rootIndex = i;
		if (httpPath == locVec[i].getPath())
		{
			if (methodIsExist(locVec[i].getMethods(),this->clientRequests[pollStruct.fd]->getMethod(),serverConfMap, pollStruct))
			{
				if (locVec[i].getRoot().empty())
					mergedPath = HelperClass::mergeDirectory(serverConfMap->getRoot(), httpPath);
				else
					mergedPath = HelperClass::mergeDirectory(locVec[i].getRoot(), httpPath);
				if (!this->clientRequests[pollStruct.fd]->getRequestFile().empty())
					mergedPath += "/" + this->clientRequests[pollStruct.fd]->getRequestFile();
				else
				{
					std::vector<std::string> indexVec;
					if (locVec[i].getIndex().size() != 0)
						indexVec = locVec[i].getIndex();
					else
						indexVec = serverConfMap->getIndex();
					if (!indexHandler(pollStruct,mergedPath, indexVec))
						return "";
				}
				break;
			}
			else
				return "";
			
		}
	}
	if (mergedPath.empty())
	{
		//buraya gitmeden de uzantıyı halletmem lazım.
		tryFiles(locVec[rootIndex], httpPath, serverConfMap, pollStruct);
		return "";
	}
	std::cout << "MERGED PATH: " << mergedPath << std::endl;
	return mergedPath;
}

void WebServer::pollOutEvent(pollfd& pollStruct)
{
	if (this->clientRequests[pollStruct.fd] == NULL)
	{
		std::cout << "this->clientRequests[pollStruct.fd] is NULL" << std::endl;
		return;
	}
	// std::string method = this->clientRequests[pollStruct.fd]->getMethod();
	this->resultPath = findRequest(pollStruct);
	if (this->clientRequests[pollStruct.fd]->getMethod() == "GET")
	{
		std::cout << "GET METHOD HANDLER" << std::endl;
		this->sendResponse(pollStruct, "200 OK");
	}
	else if (this->clientRequests[pollStruct.fd]->getMethod() == "POST")
	{
		
	}
	else if (this->clientRequests[pollStruct.fd]->getMethod() == "DELETE")
	{
		std::cout << "DELETE METHOD HANDLER" << std::endl;
		deleteMethod(pollStruct);
		delete this->clientRequests[pollStruct.fd];
	}
	else
	{
		std::cout << "UNKNOWN METHOD" << std::endl;
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
				std::cout << "POLL OUT EVENT" << std::endl;
				this->pollOutEvent(pollVec[i]);	
				pollVec[i].events = POLLIN;
			}

		}

	}
}
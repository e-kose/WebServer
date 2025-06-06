/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/06/06 19:42:25 by ekose            ###   ########.fr       */
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
		this->clientRequests = other.clientRequests;
		this->requestBuffers = other.requestBuffers;
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
	char buffer[4096];
    std::string& requestData = this->requestBuffers[pollStruct.fd];
    while (true)
    {
        int bytesReceived = recv(pollStruct.fd, buffer, sizeof(buffer), 0);

        if (bytesReceived < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            std::cerr << ">>>> Error reading from client: " << strerror(errno) << " <<<<" << std::endl;
            closeCliSocket(pollStruct.fd);
            return;
        }
        else if (bytesReceived == 0)
        {
            std::cout << ">>>> Client Ayrıldı <<<<" << std::endl;
            closeCliSocket(pollStruct.fd);
            return;
        }

        requestData.append(buffer, bytesReceived);
        size_t headerEnd = requestData.find("\r\n\r\n");
        if (headerEnd == std::string::npos)
			continue;
        this->clientRequests[pollStruct.fd] = this->parseRecv(requestData);
       	this->clientToServerMap[pollStruct.fd] = &this->searchServerConf(this->serverConfVec, this->clientRequests[pollStruct.fd]->getHostName());
		size_t maxBodySizeInBytes = this->clientToServerMap[pollStruct.fd]->getBodySize() * 1024 * 1024;
        if (requestData.size() > maxBodySizeInBytes)
        {
            sendResponse(pollStruct, "413 Payload Too Large");
            this->responseStatus = PAYLOAD_TOO_LARGE;
            return;
        }
        std::string header = requestData.substr(0, headerEnd);
        bool isChunked = header.find("Transfer-Encoding: chunked") != std::string::npos;
        size_t bodyStart = headerEnd + 4;

        if (isChunked)
        {
            std::string chunkedBody = requestData.substr(bodyStart);
            std::string unchunkedBody;
            if (!HelperClass::unchunkBody(chunkedBody, unchunkedBody))
                return;
            requestData = header + "\r\n\r\n" + unchunkedBody;
            break;
        }
        else
        {
            size_t pos = header.find("Content-Length:");
            if (pos != std::string::npos)
            {
                pos += 15;
                while (pos < header.size() && isspace(header[pos])) pos++;
                size_t end = header.find("\r\n", pos);
                std::string lengthStr = header.substr(pos, end - pos);
                size_t contentLength = std::atoi(lengthStr.c_str());

                if (requestData.size() < headerEnd + 4 + contentLength)
                    continue;
                break;
            }
            else
                break;
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
            this->requestBuffers.erase(pollStruct.fd);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing request: " << e.what() << std::endl;
            closeCliSocket(pollStruct.fd);
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
	sendMessage.clear();
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
		std::cout <<"//////////////////////////////&&&&&&&&&&&&&&&&&&&&&&&&&&\n";
		std::string sendMessage;
		this->sendResponse(pollStruct, "405 Method Not Allowed");
		this->responseStatus = METHOD_NOT_ALLOWED;
		return false;
	}
	return true;
}
std::string WebServer::tryFiles(LocationConf* locConf, const ServerConf* serverConf,  pollfd& pollStruct)
{
	std::cout << ">>>>>>>>>>>>>>>>>>> TRYFİLES <<<<<<<<<<<<<<<<<<<<<<: " << std::endl;
	std::vector<std::string> tryFilesVec = locConf->getTryFiles();
	std::string findedPath, errPage, resultDirectory;
	std::string::size_type pos1;
	bool isFind = false;
	
	std::string tryPath = HelperClass::selectLocOrServerRoot(locConf, serverConf->getRoot()) 
			+ this->clientRequests[pollStruct.fd]->getRequestFile();
	for (size_t i = 0; i < tryFilesVec.size(); i++)
	{
		pos1 = tryFilesVec[i].find("$uri");
		if (pos1 == std::string::npos && HelperClass::strIsDigit(tryFilesVec[i]))
			break ;
		if (pos1 + 4 == tryFilesVec[i].length())
			tryFilesVec[i] = tryPath;
		else
			tryFilesVec[i] = tryPath + tryFilesVec[i].substr(pos1 + 4, tryFilesVec[i].length());
		resultDirectory = tryFilesVec[i];

		// tekrar dizin kontrolune gonderilmesi lazım.
		findedPath = mergedPathHandler(resultDirectory, locConf, *serverConf, pollStruct, 2);
		if (!findedPath.empty() && this->responseStatus == NOT_RESPONDED) // listelemeye girmedi ve 404 donmeli
			isFind = true;
	}
	if (!isFind && this->responseStatus == NOT_RESPONDED)
	{
		errPage = tryFilesVec[tryFilesVec.size() -1];
		this->sendResponse(pollStruct, "404 Not Found");
		this->responseStatus = NOT_FOUND;
		return "";
	}
	return findedPath;
}

void WebServer::listDirectory(const std::string& path,LocationConf* locConf, pollfd& pollStruct)
{
	bool autoIndexCheck = locConf->getAutoIndex();
	std::cout << "----------------- LIST DIRECTORY ------------------" << std::endl;
	if (!autoIndexCheck)
	{
		this->sendResponse(pollStruct, "403 Forbidden");
		this->responseStatus = FORBIDDEN;
	}
	else
	{
		std::string httpPath = this->clientRequests[pollStruct.fd]->getPath() + this->clientRequests[pollStruct.fd]->getRequestFile();
		this->response = HelperClass::generateAutoIndexHtml(path,httpPath);
		this->sendResponse(pollStruct, "200 OK");
		this->responseStatus = OK; 
	}
}
std::string WebServer::mergedPathHandler(std::string& newMergedPath, LocationConf *locConf, const ServerConf& serverConf, pollfd& pollStruct, int recCount)
{
	// /upload sonunda / olmadan  gelirse ne olur bunu mutlaka test edicem
	// locConf da mutlaka / olmalı bunu parsta halletemem lazım
	std::cout << "================= MERGED PATH HANDLER ===============" << std::endl;
	std::cout << "Merged Path: " << newMergedPath << std::endl;

	if (HelperClass::isDirectory(newMergedPath) && recCount == 0)
	{
		std::vector <std::string> indexVec = HelperClass::selectLocOrServerIndex(locConf, serverConf.getIndex());
		std::string indexVal = HelperClass::indexHandler(newMergedPath, indexVec);
		if (!indexVal.empty())
		{
			newMergedPath += indexVal;
			return methodIsExist(locConf->getMethods(), this->clientRequests[pollStruct.fd]->getMethod(), pollStruct) ? newMergedPath : newMergedPath = "";	
		}
		else
		{
			this->listDirectory(newMergedPath, locConf, pollStruct);
			return "";
		}
	}
	else if (HelperClass::fileIsExist(newMergedPath))
		return methodIsExist(locConf->getMethods(), this->clientRequests[pollStruct.fd]->getMethod(), pollStruct) ? newMergedPath : newMergedPath = "";
	else if (recCount == 1)
	{
		std::vector<LocationConf> locVec = serverConf.getLocations();
		return tryFiles(HelperClass::findLoc("/",locVec), &serverConf, pollStruct);
	}
	if (recCount == 0 || recCount == 2)
	{
		std::string retVal = this->readHtmlFile(pollStruct, newMergedPath, serverConf);
		std::cout << "Merged Path After Read Html File: " << newMergedPath << std::endl;
		return (mergedPathHandler(newMergedPath, locConf, serverConf, pollStruct, ++recCount));
	}
	return "";
}
std::string WebServer::findRequest(pollfd& pollStruct)
{
	std::cout << "------------ FIND REQUEST ----------" << std::endl;
	ServerConf* serverConf = this->clientToServerMap[pollStruct.fd];
	std::vector<LocationConf> locVec = serverConf->getLocations();
	std::string rootPath, clientReq,  newMergedPath, httpPath, retVal;

	this->responseStatus = NOT_RESPONDED;
	httpPath = this->clientRequests[pollStruct.fd]->getPath();
	std::cout << "HTTP PATH: " << httpPath << std::endl;
	LocationConf *loc = HelperClass::findLoc(httpPath, locVec);
	rootPath = HelperClass::selectLocOrServerRoot(loc,serverConf->getRoot());
	clientReq = this->clientRequests[pollStruct.fd]->getRequestFile();	
	newMergedPath = rootPath + httpPath + clientReq;
	
	retVal = this->mergedPathHandler(newMergedPath, loc, *serverConf, pollStruct, 0);
	
	std::cout << "Finded retVal: " << retVal << std::endl;
	if (retVal.empty() && this->responseStatus == NOT_RESPONDED)
	{
		std::cout << "Merged Path Is Not Exists: " << retVal << std::endl;
		this->sendResponse(pollStruct,"404 Not Found");
		this->responseStatus = NOT_FOUND;
		return "";
	}
	return retVal;
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
	std::cout << ">>>> RESULT PATH: " << this->resultPath << "<<<<" << std::endl;
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
		std::cout << "------------ POST METHOD ---------" << std::endl;
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
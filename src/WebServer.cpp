/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:50:42 by menasy            #+#    #+#             */
/*   Updated: 2025/06/29 15:01:07 by ekose            ###   ########.fr       */
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
		this->clientKeepAlive = other.clientKeepAlive;
		this->response = other.response;
		this->responseStatus = other.responseStatus;
		this->lastActivity = other.lastActivity;
		this->headerIsParsed = other.headerIsParsed;
		this->requestHeader = other.requestHeader;
		this->requestBody = other.requestBody;
		this->unchunkedBody = other.unchunkedBody;
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

const std::map<int, ServerConf*>	 WebServer::getServerConfForFdMap()const
{
	return this->clientToServerMap;
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
				clientKeepAlive.erase(fd);
				clientRequests.erase(fd);
				clientToServerMap.erase(fd);
				requestBuffers.erase(fd);
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
	// std:: cout << "================== REQUEST ================== \n"  << request << std::endl;
	
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
		this->lastActivity[clientSock] = time(NULL);
		this->clientKeepAlive[clientSock] = false;
		this->headerIsParsed[clientSock] = false;
	}
}

bool WebServer::headerHandle(pollfd& pollStruct)
{
	std::string& requestData = this->requestBuffers[pollStruct.fd];
	size_t headerEnd = requestData.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;
	this->requestHeader = requestData.substr(0, headerEnd + 4);
	this->requestBuffers[pollStruct.fd] = requestData.substr(headerEnd + 4);
	this->headerIsParsed[pollStruct.fd] = true;
	this->clientRequests[pollStruct.fd] = this->parseRecv(requestHeader);
	this->clientToServerMap[pollStruct.fd] = 
		&this->searchServerConf(this->serverConfVec, this->clientRequests[pollStruct.fd]->getHostName());
	this->clientRequests[pollStruct.fd]->sepPath( *this->clientToServerMap[pollStruct.fd]);
	if (this->clientRequests[pollStruct.fd]->getConnection() == "keep-alive" )
		this->clientKeepAlive[pollStruct.fd] = true;
	return true;
}
bool WebServer::clientRead(pollfd& pollStruct)
{
	char buffer[131072];
	int bytesRecv;
    std::string& requestData = this->requestBuffers[pollStruct.fd];
	HttpRequest*& t = this->clientRequests[pollStruct.fd];
	while(true){
		if(headerIsParsed[pollStruct.fd]) requestData.clear();
		memset(buffer, 0, sizeof(buffer));
		bytesRecv = recv(pollStruct.fd, buffer, sizeof(buffer), 0);
		if (bytesRecv < 0) 
    		return false;
		requestData.append(buffer, bytesRecv);
		if (headerIsParsed[pollStruct.fd] == false)
		{
			std::cout << "Header is not parsed for fd: " << pollStruct.fd << std::endl;
			if (this->headerHandle(pollStruct) == false)
				continue;
			}
		this->requestBody.append(requestData);
		std::cout << "Received data for fd: " << pollStruct.fd << std::endl;
		if (t->getChunkedTransfer()){
            if (!HelperClass::unchunkBody(requestData, this->unchunkedBody)){
				if(HelperClass::requestSize(*this->clientToServerMap[pollStruct.fd], this->unchunkedBody.size()) == false){
					sendResponse(pollStruct, "413 Payload Too Large");
					closeCliSocket(pollStruct.fd);
					return false;
				}
				continue;
			}
			this->requestBody = this->unchunkedBody;
            break;
		}
		else{
			if (HelperClass::requestSize(*this->clientToServerMap[pollStruct.fd], t->getContentLength()) == false){
				sendResponse(pollStruct, "413 Payload Too Large");
				closeCliSocket(pollStruct.fd);
				return false;
			}
			std::cout << "Content Length: " << t->getContentLength() << std::endl;
			std::cout << "Request Body Size: " << this->requestBody.size() << std::endl;
			if (this->requestBody.size() < t->getContentLength())
				continue;
			}
			break;
		}
	requestData.clear();
	requestData.append(this->requestHeader + this->requestBody);
	this->clientRequests[pollStruct.fd]->addBody(this->requestBody);
	this->lastActivity[pollStruct.fd] = time(NULL);
	return true;
}
std::string WebServer::tryFiles(std::string tryPath, LocationConf* locConf, const ServerConf* serverConf,  pollfd& pollStruct, std::vector<LocationConf>& locVec)
{
	std::cout << ">>>>>>>>>>>>>>>>>>> TRYFİLES <<<<<<<<<<<<<<<<<<<<<<: " << std::endl;
	std::cout << "Try Path: " << tryPath << std::endl;
	std::vector<std::string> tryFilesVec = HelperClass::selectTryFiles(locConf, locVec);
	std::string findedPath, errPage, resultDirectory;
	std::string::size_type pos1;
	bool isFind = false;
	if (tryFilesVec.size() > 0)
	{
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
			
			findedPath = mergedPathHandler(resultDirectory, locConf, *serverConf, pollStruct, 2);
			if (!findedPath.empty() && this->responseStatus == NOT_RESPONDED) // listelemeye girmedi ve 404 donmeli
				isFind = true;
		}
	}
	if (!isFind && this->responseStatus == NOT_RESPONDED)
	{
		tryFilesVec.size() > 0 ? errPage = tryFilesVec[tryFilesVec.size() -1] : errPage = "404";
		return this->callSendResponse(pollStruct, (errPage + " Not Found"));
	}
	return findedPath;
}

void WebServer::listDirectory(const std::string& path,LocationConf* locConf, pollfd& pollStruct)
{
	std::cout << "----------------- LIST DIRECTORY ------------------" << std::endl;
	if ((locConf != NULL && locConf->getReturn().size() > 0))
	{
		std::cout << "Location Return Codes: " << std::endl;
		if (locConf->getReturn().find(301) != locConf->getReturn().end())
			this->sendResponse(pollStruct, "301 Moved Permanently");
		else if (locConf->getReturn().find(302) != locConf->getReturn().end())
			this->sendResponse(pollStruct, "302 Found");
	}
	else if (locConf != NULL && this->methodIsExist(locConf, this->clientRequests[pollStruct.fd]->getMethod(), pollStruct) == false)
		return ;
	else if (locConf == NULL || (locConf != NULL && !locConf->getAutoIndex()))
		this->sendResponse(pollStruct, "403 Forbidden");
	else
	{
		std::string httpPath = this->clientRequests[pollStruct.fd]->getPath() + this->clientRequests[pollStruct.fd]->getRequestFile();
		this->response = HelperClass::generateAutoIndexHtml(path, httpPath);
		this->sendResponse(pollStruct, "200 OK");
	}
}
std::string WebServer::mergedPathHandler(std::string& newMergedPath, LocationConf *locConf, const ServerConf& serverConf, pollfd& pollStruct, int recCount)
{
	// /upload sonunda / olmadan  gelirse ne olur bunu mutlaka test edicem
	std::cout << "================= MERGED PATH HANDLER ===============" << std::endl;
	std::cout << "Merged Path: " << newMergedPath << std::endl;
	if (HelperClass::isDirectory(newMergedPath) && (recCount == 0))
	{
		std::cout << "Merged Path is Directory: " << newMergedPath << std::endl;
		if (newMergedPath[newMergedPath.length() - 1] != '/')
			newMergedPath += '/';
		std::vector <std::string> indexVec = HelperClass::selectLocOrServerIndex(locConf, serverConf.getIndex());
		std::string indexVal = HelperClass::indexHandler(newMergedPath, indexVec);
		if (!indexVal.empty())
		{
			newMergedPath += indexVal;
			return methodIsExist(locConf, this->clientRequests[pollStruct.fd]->getMethod(), pollStruct) ? newMergedPath : newMergedPath = "";	
		}
		else
		{
			this->listDirectory(newMergedPath, locConf, pollStruct);
			return "";
		}
	}
	else if (HelperClass::fileIsExist(newMergedPath))
		return methodIsExist(locConf, this->clientRequests[pollStruct.fd]->getMethod(), pollStruct) ? newMergedPath : newMergedPath = "";
	else if (recCount == 1)
	{
		std::vector<LocationConf> locVec = serverConf.getLocations();
		return tryFiles(newMergedPath,locConf, &serverConf, pollStruct, locVec);
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
	if (httpPath.empty() || httpPath[0] != '/')
		return this->callSendResponse(pollStruct, "400 Bad Request");
	LocationConf *loc = HelperClass::findLoc(httpPath, locVec);
	rootPath = HelperClass::selectLocOrServerRoot(loc,serverConf->getRoot());
	clientReq = this->clientRequests[pollStruct.fd]->getRequestFile();	
	newMergedPath = rootPath + httpPath + clientReq;
	
	retVal = this->mergedPathHandler(newMergedPath, loc, *serverConf, pollStruct, 0);
	
	std::cout << "Finded retVal: " << retVal << std::endl;
	if (retVal.empty() && this->responseStatus == NOT_RESPONDED)
	{
		std::cout << "Merged Path Is Not Exists: " << retVal << std::endl;
		return this->callSendResponse(pollStruct,"404 Not Found");
	}
	else if (!retVal.empty() && (loc != NULL && loc->getReturn().size() > 0))
	{
		if (loc->getReturn().find(301) != loc->getReturn().end())
			this->sendResponse(pollStruct, "301 Moved Permanently");
		else if (loc->getReturn().find(302) != loc->getReturn().end())
			this->sendResponse(pollStruct, "302 Found");
	}
	return this->pathCheck(retVal,rootPath,pollStruct);
}

bool WebServer::checkTimeouts() {
    time_t now = time(NULL);
	bool check = false;
    for (std::map<int, time_t>::iterator it = lastActivity.begin(); it != lastActivity.end(); ) {
        int fd = it->first;
        time_t last = it->second;
		// std::cout << "Checking timeout for fd: " << fd << std::endl;
        if (now - last > TIMEOUT_SEC) {
            std::cout << ">>>> Timeout. Closing idle connection: " << fd << std::endl;
            closeCliSocket(fd);
			lastActivity.erase(it++);
			check = true;
        } else {
            ++it;
        }
    }
	return check;
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
		std::cout << "------------- GET METHOD --------------- "<< std::endl;
		this->sendResponse(pollStruct, "200 OK");
	}
	else if (this->clientRequests[pollStruct.fd]->getMethod() == "POST")
	{
		std::cout << "------------ POST METHOD ---------" << std::endl;
		this->sendResponse(pollStruct, "200 OK");

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
		int result = poll(pollVec.data(), pollVec.size(), 1000);
		if (result < 0)
			throw std::runtime_error("poll() error. Terminating server.");
		if(checkTimeouts())continue;
		for (size_t i = 0; i < pollVec.size(); i++)
		{
			std::cout << "FD " << pollVec[i].fd << " events: " << pollVec[i].events 
          << " revents: " << pollVec[i].revents << std::endl;
			if (pollVec[i].revents & POLLERR) 
			{
        		HelperClass::writeToFile(this->clientToServerMap[pollVec[i].fd]->getErrorLog(), "POLL ERROR");
        		closeCliSocket(pollVec[i].fd);
        		continue;
    		}
			
			if (this->socketMap.count(pollVec[i].fd) && (pollVec[i].revents & POLLIN))
			{
				this->acceptNewClient(pollVec[i]);
				std::cout << "new client accepted on socket: " << pollVec[i].fd << std::endl;
				continue;
			}

			if (pollVec[i].revents & POLLIN)
			{
				if(this->clientRead(pollVec[i]) == false){
					pollVec[i].events = POLLIN;
					continue;
				}
				pollVec[i].events = POLLOUT;
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
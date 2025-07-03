/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodTransaction.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 07:52:01 by ekose             #+#    #+#             */
/*   Updated: 2025/07/01 16:55:55 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

void WebServer::sendResponse(const int &pollIndex, const std::string &status)
{
	int fd = this->pollVec[pollIndex].fd;
	size_t pos = status.find(" ");
	int code = std::atoi(status.substr(0, pos).c_str());
	std::string log = socketInfo(clientToAddrMap[fd], fd) + " " + this->clientRequests[this->pollVec[pollIndex].fd]->getMethod() + " " + this->clientRequests[this->pollVec[pollIndex].fd]->getPath() + this->clientRequests[this->pollVec[pollIndex].fd]->getRequestFile() + " " + status.substr(0, pos);
	HelperClass::writeToFile(this->clientToServerMap[fd]->getAccessLog(), log);
	if (code >= 400)
		this->response = this->createErrorResponse(pollIndex, status, *clientToServerMap[fd], clientToServerMap[fd]->getRoot());
	else if (code == 301 || code == 302)
		this->response = redirectResponse(pollIndex, status.substr(0, pos), status.substr(pos + 1), "text/html");
	else if (code >= 200 && code <= 205)
	{
		std::string httpMethod = this->clientRequests[this->pollVec[pollIndex].fd]->getMethod();
		std::string contentType = HelperClass::findContentType(this->resultPath);
		if ((httpMethod == "GET" || httpMethod == "POST") && this->response.empty())
			this->response = this->createHttpResponse(pollIndex, status.substr(0, pos), "OK", contentType, this->readHtmlFile(pollIndex, this->resultPath, *this->clientToServerMap[fd]));
		else if (httpMethod == "DELETE" && this->response.empty())
		{
			std::string filePath = HelperClass::indexIsExist(*clientToServerMap[fd], this->clientRequests[this->pollVec[pollIndex].fd]->getPath());
			this->response = this->createHttpResponse(pollIndex, status.substr(0, pos), "OK", contentType,
													  this->readHtmlFile(pollIndex,
																		 filePath,
																		 *this->clientToServerMap[fd]));
		}
	}
	this->responseStatus = code;
	std::cout << "Response Code: " << this->responseStatus << std::endl;
	sendHandler(pollIndex, this->response);
}

bool isPathUnderRoot(const std::string &path, const std::string &root)
{
	return path.find(root) == 0;
}

void WebServer::deleteMethod(const int &pollIndex)
{
	std::string finalPath;
	if (!HelperClass::resolvePath(this->resultPath, finalPath))
	{
		sendResponse(pollIndex, "404 Not Found");
		return;
	}

	if (access(finalPath.c_str(), W_OK) != 0)
	{
		sendResponse(pollIndex, "403 Forbidden");
		return;
	}

	// Root belirleme
	std::string allowedRoot;
	std::vector<LocationConf> locations = clientToServerMap[this->pollVec[pollIndex].fd]->getLocations();
	LocationConf *locConf = HelperClass::findLoc(this->clientRequests[this->pollVec[pollIndex].fd]->getPath(), locations);
	allowedRoot = HelperClass::selectLocOrServerRoot(locConf, clientToServerMap[this->pollVec[pollIndex].fd]->getRoot());

	if (allowedRoot.empty())
	{
		sendResponse(pollIndex, "403 Forbidden");
		return;
	}

	std::string resolvedRoot;
	if (!HelperClass::resolvePath(allowedRoot, resolvedRoot))
	{
		sendResponse(pollIndex, "500 Internal Server Error");
		return;
	}

	if (!isPathUnderRoot(finalPath, resolvedRoot))
	{
		sendResponse(pollIndex, "403 Forbidden");
		return;
	}

	struct stat st;
	if (stat(finalPath.c_str(), &st) != 0)
	{
		sendResponse(pollIndex, "404 Not Found");
		return;
	}

	if (!S_ISREG(st.st_mode))
	{
		sendResponse(pollIndex, "403 Forbidden");
		return;
	}

	if (remove(finalPath.c_str()) != 0)
	{
		sendResponse(pollIndex, "500 Internal Server Error");
		return;
	}
	sendResponse(pollIndex, "200 OK");
}

void WebServer::sendHandler(const int &pollIndex, std::string &sendMessage)
{
	size_t totalSend = 0;
	size_t lengthMessage = sendMessage.size();

	while (totalSend < lengthMessage)
	{
		ssize_t sendVal = send(this->pollVec[pollIndex].fd, sendMessage.c_str() + totalSend, lengthMessage - totalSend, 0);

		if (sendVal == -1)
		{
			if (this->retryCountMap[this->pollVec[pollIndex].fd] < MAX_RETRY_COUNT)
			{
				this->retryCountMap[this->pollVec[pollIndex].fd]++;
				this->pollVec[pollIndex].events = POLLOUT;
			}
			else 
				this->closeCliSocket(this->pollVec[pollIndex].fd);
			break;
		}
		else
			totalSend += sendVal;
	}
	if (totalSend == lengthMessage)
	{
		sendMessage.clear();
		if (this->clientKeepAlive[this->pollVec[pollIndex].fd])
			this->pollVec[pollIndex].events = POLLIN;
		else
			this->closeCliSocket(this->pollVec[pollIndex].fd);
	}
	else
		this->pollVec[pollIndex].events = POLLOUT;
	this->cleanReq(pollIndex);
}

bool WebServer::methodIsExist(LocationConf *locConf, const std::string &requestMethod, const int &pollIndex)
{
	bool check = false;
	if (locConf != NULL)
	{
		std::vector<std::string> locMethodsvec = locConf->getMethods();
		for (size_t i = 0; i < locMethodsvec.size(); i++)
		{
			if (locMethodsvec[i] == requestMethod)
				check = true;
		}
	}
	else
		requestMethod == "GET" ? check = true : check = false;

	if (!check)
	{
		this->sendResponse(pollIndex, "405 Method Not Allowed");
		return false;
	}
	return true;
}

void WebServer::cleanReq(const int &pollIndex)
{
	this->requestBuffers.erase(this->pollVec[pollIndex].fd);
	this->headerIsParsed[this->pollVec[pollIndex].fd] = false;
	this->requestBody.clear();
	this->unchunkedBody.clear();
	this->requestHeader.clear();
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodTransaction.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 07:52:01 by ekose             #+#    #+#             */
/*   Updated: 2025/06/28 21:01:01 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

void  WebServer::sendResponse(pollfd& pollStruct, const std::string& status)
{
	int			fd = pollStruct.fd;
	size_t		pos = status.find(" ");
	int			code = std::atoi(status.substr(0,pos).c_str());
	std::cout << this->clientRequests[pollStruct.fd]<< std::endl;
	std::string	log = socketInfo(clientToAddrMap[fd], fd) + " " 
						+ this->clientRequests[pollStruct.fd]->getMethod()+ " " 
						+ this->clientRequests[pollStruct.fd]->getPath() + "/"
						+ this->clientRequests[pollStruct.fd]->getRequestFile() 
						+ " " + status.substr(0,pos);	
	HelperClass::writeToFile("access.log",log);
	std::cout << ">>>> RESPONSE CODE: " << code << "<<<<" << std::endl;
	if (code >= 400)
		this->response = this->createErrorResponse(pollStruct,status, *clientToServerMap[fd], clientToServerMap[fd]->getRoot());
	else if (code == 301 || code == 302)
		this->response = redirectResponse(pollStruct, status.substr(0, pos), status.substr(pos + 1), "text/html");
	else if(code >= 200 && code <= 205)
	{
		std::string httpMethod = this->clientRequests[pollStruct.fd]->getMethod();
		if ((httpMethod == "GET"  || httpMethod == "POST") && this->response.empty())
			this->response = this->createHttpResponse(pollStruct,status.substr(0, pos), "OK", "text/html", this->readHtmlFile(pollStruct,this->resultPath, *this->clientToServerMap[fd]));
		else if (httpMethod == "DELETE" && this->response.empty())
		{
			std::string filePath =HelperClass::indexIsExist(*clientToServerMap[fd],this->clientRequests[pollStruct.fd]->getPath());
			this->response = this->createHttpResponse(pollStruct,status.substr(0, pos), "OK", "text/html",
												this->readHtmlFile(pollStruct,
												filePath,
												*this->clientToServerMap[fd]));
		}
			
	}
	this->responseStatus = code;
	sendHandler(pollStruct, this->response);
}



bool isPathUnderRoot(const std::string& path, const std::string& root) {
	return path.find(root) == 0;
}

void WebServer::deleteMethod(pollfd& pollStruct)
{
	std::string finalPath;
	if (!HelperClass::resolvePath(this->resultPath, finalPath)) 
	{
		sendResponse(pollStruct, "404 Not Found");
		return;
	}

	if (access(finalPath.c_str(), W_OK) != 0) {
		sendResponse(pollStruct, "403 Forbidden");
		return;
	}

	// Root belirleme
	std::string allowedRoot;
	std::cout << "DELETE RESULT: " << this->resultPath << std::endl;
	std::vector<LocationConf> locations = clientToServerMap[pollStruct.fd]->getLocations();
	LocationConf* locConf = HelperClass::findLoc(this->clientRequests[pollStruct.fd]->getPath(), locations);
	allowedRoot = HelperClass::selectLocOrServerRoot(locConf, clientToServerMap[pollStruct.fd]->getRoot());

	
	std::cout << "ALLOWEDROOT: " << allowedRoot << std::endl;
	if (allowedRoot.empty()) {
		sendResponse(pollStruct, "403 Forbidden");
		return;
	}

	std::string resolvedRoot;
	if (!HelperClass::resolvePath(allowedRoot, resolvedRoot)) {
		sendResponse(pollStruct, "500 Internal Server Error");
		return;
	}

	if (!isPathUnderRoot(finalPath, resolvedRoot)) {
		sendResponse(pollStruct, "403 Forbidden");
		return;
	}

	struct stat st;
	if (stat(finalPath.c_str(), &st) != 0) {
		sendResponse(pollStruct, "404 Not Found");
		return;
	}

	if (!S_ISREG(st.st_mode)) {
		sendResponse(pollStruct, "403 Forbidden");
		return;
	}

	if (remove(finalPath.c_str()) != 0) {
		sendResponse(pollStruct, "500 Internal Server Error");
		return;
	}
	sendResponse(pollStruct, "200 OK");
}
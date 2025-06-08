/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodTransaction.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 07:52:01 by ekose             #+#    #+#             */
/*   Updated: 2025/06/08 09:50:40 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

void  WebServer::sendResponse(pollfd& pollStruct, const std::string& status)
{
	int			fd = pollStruct.fd;
	size_t		pos = status.find(" ");
	int			code = std::atoi(status.substr(0,pos).c_str());
	std::string	log = socketInfo(clientToAddrMap[fd], fd) + " " 
						+ this->clientRequests[pollStruct.fd]->getMethod()+ " " 
						+ this->clientRequests[pollStruct.fd]->getPath() + "/"
						+ this->clientRequests[pollStruct.fd]->getRequestFile() 
						+ " " + status.substr(0,pos);
	HelperClass::writeToFile("access.log",log);
	std::cout<< " RESPONSE DEGER:" << this->resultPath << std::endl;
	std::cout << ">>>> RESPONSE CODE: " << code << "<<<<" << std::endl;
	if (code >= 400)
		this->response = this->createErrorResponse(pollStruct,status, *clientToServerMap[fd], clientToServerMap[fd]->getRoot());
	else if (code == 301 || code == 302)
		this->response = redirectResponse(pollStruct, status.substr(0, pos), status.substr(pos + 1), "text/html");
	else if(code >= 200 && code <= 205)
	{
		std::string httpMethod = this->clientRequests[pollStruct.fd]->getMethod();
		if (httpMethod == "GET" && this->response.empty())
			this->response = this->createHttpResponse(status.substr(0, pos), "OK", "text/html", this->readHtmlFile(pollStruct,this->resultPath, *this->clientToServerMap[fd]));
		else if (httpMethod == "DELETE" && this->response.empty())
		{
			std::string filePath =HelperClass::indexIsExist(*clientToServerMap[fd],this->clientRequests[pollStruct.fd]->getPath());
			this->response = this->createHttpResponse(status.substr(0, pos), "OK", "text/html",
												this->readHtmlFile(pollStruct,
												filePath,
												*this->clientToServerMap[fd]));
		}
			
	}
	sendHandler(pollStruct, this->response);
}

bool resolvePath(const std::string& path, std::string& out) {
	char resolved[PATH_MAX];
	if (realpath(path.c_str(), resolved) == NULL)
		return false;
	out = resolved;
	return true;
}

bool isPathUnderRoot(const std::string& path, const std::string& root) {
	return path.find(root) == 0;
}

void WebServer::deleteMethod(pollfd& pollStruct)
{
	std::string finalPath;
	if (!resolvePath(this->resultPath, finalPath)) 
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
	for (size_t i = 0; i < locations.size(); ++i) {
		if (this->clientRequests[pollStruct.fd]->getPath() == locations[i].getPath())
		{
			allowedRoot = locations[i].getRoot().empty() ?
			              clientToServerMap[pollStruct.fd]->getRoot() :
			              locations[i].getRoot();
			break;
		}
	}
	
	std::cout << "ALLOWEDROOT: " << allowedRoot << std::endl;
	if (allowedRoot.empty()) {
		sendResponse(pollStruct, "403 Forbidden");
		return;
	}

	std::string resolvedRoot;
	if (!resolvePath(allowedRoot, resolvedRoot)) {
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
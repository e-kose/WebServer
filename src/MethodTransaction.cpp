/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodTransaction.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 07:52:01 by ekose             #+#    #+#             */
/*   Updated: 2025/05/12 10:17:47 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

void  WebServer::sendResponse(HttpRequest* httpReques, int fd, const std::string& status)
{
	std::string response;
	size_t		pos = status.find(" ");
	int			errorCode = std::atoi(status.substr(0,pos).c_str());
	std::string	log = socketInfo(clientToAddrMap[fd], fd) + " " 
						+ httpReques->getMethod()+ " " 
						+ httpReques->getPath() + "/" 
						+ httpReques->getRequestFile() 
						+ " " + status.substr(0,pos) + "\n";
	HelperClass::writeToFile("access.log",log);
	if (errorCode >= 400)
		response = HelperClass::createErrorResponse(status, *clientToServerMap[fd], clientToServerMap[fd]->getRoot());
	
	else
	{
		response += "<html><body><h1>" + status + "</h1></body></html>";
	}
	send(fd, response.c_str(), response.size(), 0);
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

void WebServer::deleteMethod(HttpRequest* httpRequest, pollfd& pollStruct)
{
	std::string path = findRequest(httpRequest, pollStruct);
	std::string finalPath;

	if (!resolvePath(path, finalPath)) {
		sendResponse(httpRequest,pollStruct.fd, "404 Not Found");
		return;
	}

	if (access(finalPath.c_str(), W_OK) != 0) {
		sendResponse(httpRequest,pollStruct.fd, "403 Forbidden");
		return;
	}

	// Root belirleme
	std::string allowedRoot;
	std::vector<LocationConf> locations = clientToServerMap[pollStruct.fd]->getLocations();
	for (size_t i = 0; i < locations.size(); ++i) {
		if (httpRequest->getPath() == locations[i].getPath())
		{
			allowedRoot = locations[i].getRoot().empty() ?
			              clientToServerMap[pollStruct.fd]->getRoot() :
			              locations[i].getRoot();
			break;
		}
	}
	
	if (allowedRoot.empty()) {
		sendResponse(httpRequest,pollStruct.fd, "403 Forbidden");
		return;
	}

	std::string resolvedRoot;
	if (!resolvePath(allowedRoot, resolvedRoot)) {
		sendResponse(httpRequest,pollStruct.fd, "500 Internal Server Error");
		return;
	}

	if (!isPathUnderRoot(finalPath, resolvedRoot)) {
		sendResponse(httpRequest,pollStruct.fd, "403 Forbidden");
		return;
	}

	struct stat st;
	if (stat(finalPath.c_str(), &st) != 0) {
		sendResponse(httpRequest,pollStruct.fd, "404 Not Found");
		return;
	}

	if (!S_ISREG(st.st_mode)) {
		sendResponse(httpRequest,pollStruct.fd, "403 Forbidden");
		return;
	}

	if (remove(finalPath.c_str()) != 0) {
		sendResponse(httpRequest,pollStruct.fd, "500 Internal Server Error");
		return;
	}

	sendResponse(httpRequest,pollStruct.fd, "200 OK");
}
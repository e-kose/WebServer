/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodTransaction.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 07:52:01 by ekose             #+#    #+#             */
/*   Updated: 2025/05/13 11:09:34 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

void  WebServer::sendResponse(pollfd& pollStruct, const std::string& status)
{
	int			fd = pollStruct.fd;
	std::string response;
	size_t		pos = status.find(" ");
	int			code = std::atoi(status.substr(0,pos).c_str());
	std::string	log = socketInfo(clientToAddrMap[fd], fd) + " " 
						+ this->clientRequests[pollStruct.fd]->getMethod()+ " " 
						+ this->clientRequests[pollStruct.fd]->getPath() + "/" 
						+ this->clientRequests[pollStruct.fd]->getRequestFile() 
						+ " " + status.substr(0,pos);
	HelperClass::writeToFile("access.log",log);
	if (code >= 400)
		response = HelperClass::createErrorResponse(status.substr(0, pos), *clientToServerMap[fd], clientToServerMap[fd]->getRoot());
	else if(code >= 200 && code <= 205)
	{
		std::string httpMethod = this->clientRequests[pollStruct.fd]->getMethod();
		if (httpMethod == "GET")
			response = HelperClass::createHttpResponse(status.substr(0, pos), "OK", "text/html", HelperClass::readHtmlFile(this->resultPath));
		else if (httpMethod == "DELETE")
			response = HelperClass::createHttpResponse(status.substr(0, pos), "OK", "text/html", HelperClass::readHtmlFile(clientToServerMap[fd]->getRoot() + "/index.html"));
	}
		//index vectoruna bakılacak
	sendHandler(pollStruct, response);
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

	if (!resolvePath(this->resultPath, finalPath)) {
		sendResponse(pollStruct, "404 Not Found");
		return;
	}

	if (access(finalPath.c_str(), W_OK) != 0) {
		sendResponse(pollStruct, "403 Forbidden");
		return;
	}

	// Root belirleme
	std::string allowedRoot;
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
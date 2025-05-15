/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 21:40:13 by menasy            #+#    #+#             */
/*   Updated: 2025/05/15 17:27:41 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

std::string WebServer::findLocation(const ServerConf& conf, std::string locStr)
{
	std::vector<LocationConf> locVec = conf.getLocations();
	for (size_t i = 0; i < locVec.size(); i++)
	{
		if (locVec[i].getPath() == locStr)
			return locVec[i].getCgiExtension();
	}
	return "";
}

std::string WebServer::readHtmlFile(pollfd& pollStruct, const std::string& path, const ServerConf& conf) 
{
	std::cout << "READ HTML FILE: " << path << std::endl;
	if (path.empty())
		return "";
    std::ifstream file(path.c_str());
	std::string pathWithExt, locationCgiExt;
	if (!file.is_open()) 
	{
		locationCgiExt = findLocation(conf,"/cgi-bin");
		if (locationCgiExt.empty())
			return "";
		pathWithExt = HelperClass::checkFileWithExtension(path, locationCgiExt);
		std::cout << "EXTENSION VAL: " << pathWithExt << std::endl;
        if (pathWithExt == path)
			return "";
		else
		{
			std::string ext = pathWithExt.substr(pathWithExt.find_last_of("."), pathWithExt.length());
			std::cout << "EXTENSION CGİ2: " << ext << std::endl;
			int isExecInt = HelperClass::fileIsExecutable(path, ext, locationCgiExt);
			
			if (isExecInt == 0)
			{
				file.open(pathWithExt.c_str());
				if (!file.is_open())
					return "";
			}
			else if (isExecInt == -1)
			{
				std::cout << "------------------ Cgi Extensions yok ------------------\n";
				this->sendResponse(pollStruct, "403 Forbidden");
				return "";
			}
			else if (isExecInt == 1)
			{
				//ext i cgi içinde hangi pathe gore nerde derleneceğini bulmak için aldım ona gpre
				//mapden hangi değeri çekeceğimi bulacağım.
				std::cout << "---------------- CGI YA GONFERİLDİ --------------\n";
				// this->sendCgi(path, ext);
			}	
		}
    }
	std::cout << "FILE IS OPENED" << std::endl;
    std::stringstream buffer;
    buffer << file.rdbuf();
	file.close();

	return buffer.str();
}

std::string WebServer::createHttpResponse(
	const std::string& statusCode, const std::string& statusMessage,
	const std::string& contentType, const std::string& body) 
{
	std::ostringstream ss;
	ss << body.size();	
	std::string response = "HTTP/1.1 " + statusCode + " " + statusMessage + "\r\n";
	response += "Content-Type: " + contentType + "\r\n";
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "Connection: close\r\n"; 
	response += "\r\n";
	response += body;	
	return response;
}

std::string WebServer::createErrorResponse(pollfd& pollStruct, const std::string& status, const ServerConf& conf, const std::string& rootPAth)
{
	std::string content;
	size_t pos = status.find_first_of(" ");
	std::string statusCode = status.substr(0, pos);
	std::string statusMessage = status.substr(pos + 1);
	std::map<int, std::string> errMap = conf.getErrorPages();
	std::map<int, std::string> defaultErrMap = conf.getDfltErrPage();
	int errCode = std::atoi(status.c_str());
	std::cout << "ERROR CODE>>>>>>>>>>>>>>>>>: " << errCode << std::endl;
	if (errMap.find(errCode) != errMap.end())
	{
		std::cout << "ERROR PAGE????: " << errMap[errCode] << std::endl;
		content = readHtmlFile(pollStruct,HelperClass::mergeDirectory(rootPAth, errMap[errCode]),conf);
		if (!content.empty())
			return createHttpResponse(statusCode, statusMessage, "text/html", content);
	}
	return createHttpResponse(statusCode, statusMessage, "text/html",defaultErrMap[errCode]);
}

// std::string WebServer::sendCgi(const std::string&filePath, const std::string& fileExt)
// {
	
// }

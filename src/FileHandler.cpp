/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 21:40:13 by menasy            #+#    #+#             */
/*   Updated: 2025/05/14 00:43:01 by menasy           ###   ########.fr       */
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

std::string WebServer::readHtmlFile(const std::string& path, const ServerConf& conf) 
{
	std::cout << "READ HTML FILE: " << path << std::endl;
	if (path.empty())
		return "";
    std::ifstream file(path.c_str());
	std::string extensionVal, locationCgiExt;
	if (!file.is_open()) 
	{
		locationCgiExt = findLocation(conf,"/cgi-bin");
		if (locationCgiExt.empty())
			return "";
		std::cout << "LOCATION CGI EXT: " << locationCgiExt << std::endl;
		extensionVal = HelperClass::checkFileWithExtension(path, locationCgiExt);
		std::cout << "EXTENSION VAL: " << extensionVal << std::endl;
        if (extensionVal == path)
			return "";
		else
		{

			std::string ext = extensionVal.substr(extensionVal.find_last_of("."), extensionVal.length());
			if (HelperClass::fileIsExecutable(path, ext, locationCgiExt))
			{
				std::cout << "---------------- CGI YA GONFERİLDİ --------------\n";
				// this->sendCgi(path, ext);
			}
			else
			{
				file.open(extensionVal.c_str());
				if (!file.is_open())
					return "";
			}	
		}
    }
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

std::string WebServer::createErrorResponse(const std::string& status, const ServerConf& conf, const std::string& rootPAth)
{
	std::string content;
	size_t pos = status.find_first_of(" ");
	std::string statusCode = status.substr(0, pos);
	std::string statusMessage = status.substr(pos + 1);
	std::map<int, std::string> errMap = conf.getErrorPages();
	std::map<int, std::string> defaultErrMap = conf.getDfltErrPage();
	int errCode = std::atoi(status.c_str());
	
	if (errMap.find(errCode) != errMap.end())
	{
		content = readHtmlFile(HelperClass::mergeDirectory(rootPAth, errMap[errCode]),conf);
		if (!content.empty())
			return createHttpResponse(statusCode, statusMessage, "text/html", content);
	}
	return createHttpResponse(statusCode, statusMessage, "text/html",defaultErrMap[errCode]);
}

// std::string WebServer::sendCgi(const std::string&filePath, const std::string& fileExt)
// {
	
// }

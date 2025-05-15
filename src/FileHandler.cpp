/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 21:40:13 by menasy            #+#    #+#             */
/*   Updated: 2025/05/15 21:37:56 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

std::map<std::string, std::string> WebServer::findLocation(const ServerConf& conf, std::string locStr)
{
	std::vector<LocationConf> locVec = conf.getLocations();
	std::map<std::string, std::string> cgiExtMap;
	for (size_t i = 0; i < locVec.size(); i++)
	{
		if (locVec[i].getPath() == locStr)
			cgiExtMap = locVec[i].getCgiExt();
	}
	return cgiExtMap;
}

std::string WebServer::readHtmlFile(pollfd& pollStruct, const std::string& path, const ServerConf& conf) 
{
	std::cout << "-------------- READ HTML FILE : " << path <<" --------------"<<std::endl;
	if (path.empty())
		return "";
	std::string newPath;
	std::map <std::string, std::string> cgiMap;
	cgiMap = findLocation(conf,"/cgi-bin");
	newPath = HelperClass::checkFileWithExtension(path, cgiMap);
    std::ifstream file(newPath.c_str());
	if (file.is_open()) 
	{
		std::size_t pos = newPath.find_last_of(".");
		if (pos != std::string::npos)
		{
			std::string ext = newPath.substr(pos, newPath.length());
			int isExecInt = HelperClass::fileIsExecutable(newPath, ext, cgiMap);
			if (isExecInt == -1)
			{
				std::cout << ">>>> Cgi Extensions YOK -<<<<\n";
				file.close();
				this->sendResponse(pollStruct, "403 Forbidden");
				return "Forbidden";
			}
			else if (isExecInt == 1)
			{
				std::cout << ">>>> CGI YA GONFERİLDİ <<<<\n" << ext << std::endl;
				//ext i cgi içinde hangi pathe gore nerde derleneceğini bulmak için aldım ona gpre
				//mapden hangi değeri çekeceğimi bulacağım.
				// this->sendCgi(path, ext);
			}	
		}
    }
	else
		return "";
	std::cout << ">>> FILE IS OPENED <<<" << std::endl;
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
	std::cout << ">>>> ERROR CODE: " << errCode << " <<<<" << std::endl;
	if (errMap.find(errCode) != errMap.end())
	{
		content = readHtmlFile(pollStruct,HelperClass::mergeDirectory(rootPAth, errMap[errCode]),conf);
		if (!content.empty() && content != "Forbidden")
			return createHttpResponse(statusCode, statusMessage, "text/html", content);
	}
	return createHttpResponse(statusCode, statusMessage, "text/html",defaultErrMap[errCode]);
}

// std::string WebServer::sendCgi(const std::string&filePath, const std::string& fileExt)
// {
	
// }

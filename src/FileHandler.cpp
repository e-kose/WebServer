/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 21:40:13 by menasy            #+#    #+#             */
/*   Updated: 2025/05/28 13:30:24 by menasy           ###   ########.fr       */
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
	std::map<int, std::string> defaultErrMap = conf.getDfltPage();
	int errCode = std::atoi(status.c_str());
	std::cout << ">>>> ERROR CODE: " << errCode << " <<<<" << std::endl;
	if (errMap.find(errCode) != errMap.end())
	{
		std::string res = HelperClass::mergeDirectory(rootPAth, errMap[errCode]);
		content = readHtmlFile(pollStruct,res,conf);
		if (!content.empty() && content != "Forbidden")
			return createHttpResponse(statusCode, statusMessage, "text/html", content);
	}
	return createHttpResponse(statusCode, statusMessage, "text/html",defaultErrMap[errCode]);
}
std::string WebServer::readHtmlFile(pollfd& pollStruct,std::string& path, const ServerConf& conf) 
{
	std::cout << "-------------- READ HTML FILE : " << path <<" --------------"<<std::endl;
	if (path.empty())
		return "";
	if (path == "errorPage")
		return conf.getDfltPage().at(404);
	std::string newPath;
	std::map <std::string, std::string> cgiMap;
	cgiMap = findLocation(conf,"/cgi-bin");
	newPath = HelperClass::checkFileWithExtension(path, cgiMap);
    std::ifstream file(newPath.c_str());
	std::cout << ">>>> FILE PATH: " << newPath << "<<<<\n";
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
				std::string scriptContetnt;
				std::cout << ">>>> CGI YA GONFERİLDİ <<<<\n" << ext << std::endl;
				//mapden hangi değeri çekeceğimi bulacağım.
				scriptContetnt = this->sendCgi(newPath, ext, pollStruct,conf, cgiMap);
				file.close();
				return scriptContetnt;
			}	
		}
    }
	else
		return "";
	std::cout << ">>> FILE IS OPENED <<<" << std::endl;
    std::stringstream buffer;
    buffer << file.rdbuf();
	file.close();
	path = newPath;
	return buffer.str();
}
std::vector<char *> WebServer::fillEnv(const ServerConf& conf, const pollfd& pollStruct, const std::string& path)
{
	std::vector<char *> env;
	std::vector<std::string> envVec;
	const int& fd = pollStruct.fd;
	envVec.push_back("REQUEST_METHOD=" + this->clientRequests[fd]->getMethod());
	envVec.push_back("SCRIPT_NAME=" + path);
	envVec.push_back("QUERY_STRING=" + this->clientRequests[fd]->getQueryString());
	envVec.push_back("CONTENT_TYPE=" + this->clientRequests[fd]->getContentType());
	envVec.push_back("CONTENT_LENGTH=" + HelperClass::intToString(this->clientRequests[fd]->getContentLength()));
	envVec.push_back("SERVER_PROTOCOL=" + this->clientRequests[fd]->getVersion());
	envVec.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVec.push_back("SERVER_SOFTWARE=WebServer/1.0");
	envVec.push_back("SERVER_NAME=" + this->clientRequests[fd]->getHostName());
	envVec.push_back("SERVER_PORT=" + HelperClass::intToString(conf.getPort()));
	envVec.push_back("REMOTE_ADDR=" + this->socketInfo(this->clientToAddrMap[fd], fd));
	envVec.push_back("REDIRECT_STATUS=200");

	for (size_t i = 0; i < envVec.size(); i++)
	{
		env.push_back(const_cast<char *>(envVec[i].c_str()));
	}
	env.push_back(NULL);
	return env;
}
// std::string WebServer::postHandler(const std::string&filePath, std::string& cgiPath)
// {
// 	int cinFd[2];
// 	int outFd[2];

// 	if (pipe(cinFd) == -1 || pipe(outFd) == -1)
// 	{
// 		return "";
// 	}
// 	pid_t pid = fork();
// 	if (pid == -1)
// 	{
// 		std::cout << "Fork error" << std::endl;
// 		return "";
// 	}
// 	else if (pid == 0)
// 	{
// 		close(fd[0]);
// 		dup2(fd[1], STDOUT_FILENO);
// 		close(fd[1]);
// 		char* argv[] = {
// 			const_cast<char*>(cgiExecPath.c_str()),  
// 			const_cast<char*>(filePath.c_str()),     
// 			NULL
// 		};
// 		execve(cgiExecPath.c_str(),argv,env.data());
// 		throw std::runtime_error("Execve failed");
// 	}
// }

std::string WebServer::sendCgi(const std::string&filePath, std::string& fileExt, const pollfd& pollStruct, const ServerConf& conf, const std::map<std::string,std::string>&cgiExtMap)
{
	std::string cgiExecPath = cgiExtMap.at(fileExt);
	std::string scriptContent;
	std::cout << ">>>> CGI EXEC PATH: " << cgiExecPath << "<<<<\n";
	std::cout << ">>>> CGI FILE PATH: " << filePath << "<<<<\n";
	std::cout  << "Fd: " << pollStruct.fd << std::endl;
	std::vector<char *> env = this->fillEnv(conf, pollStruct, filePath);
	// if (this->clientRequests[pollStruct.fd]->getMethod() == "POST")
	// 	postHandler(filePath, cgiExecPath);
	int fd[2];
	if (pipe(fd) == -1)
	{
		//log yazak
		std::cout << "Pipe error" << std::endl;
		return "";
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		std::cout << "Fork error" << std::endl;
		return "";
	}
	else if (pid == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		char* argv[] = {
			const_cast<char*>(cgiExecPath.c_str()),  
			const_cast<char*>(filePath.c_str()),     
			NULL
		};
		execve(cgiExecPath.c_str(),argv,env.data());
		throw std::runtime_error("Execve failed");
	}
	
	close(fd[1]);
	waitpid(pid, NULL, 0);
	scriptContent = HelperClass::fdToString(fd[0]);
	close(fd[0]);
	std::cout << ">>>> CGI SCRIPT CONTENT: " << scriptContent << "<<<<\n";
	return scriptContent;
}

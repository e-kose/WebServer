/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 21:40:13 by menasy            #+#    #+#             */
/*   Updated: 2025/06/07 16:26:38 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

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
		if (!content.empty() )
			return createHttpResponse(statusCode, statusMessage, "text/html", content);
	}
	return createHttpResponse(statusCode, statusMessage, "text/html",defaultErrMap[errCode]);
}

std::string WebServer::redirectResponse(pollfd& pollStruct,
										const std::string& statusCode,
										const std::string& statusMessage,
										const std::string& contentType)
{
	std::vector<LocationConf> locations = this->clientToServerMap[pollStruct.fd]->getLocations();
	LocationConf* locConf = HelperClass::findLoc(this->resultPath, locations);
	std::map<int, std::string> retMap = locConf->getReturn();

	int code = std::atoi(statusCode.c_str());

	std::string response = "HTTP/1.1 " + statusCode + " " + statusMessage + "\r\n";
	response += "Content-Type: " + contentType + "\r\n";

	if (retMap.find(code) != retMap.end())
		response += "Location: " + retMap[code] + "\r\n";
	else
		response += "Location: /\r\n";

	response += "Content-Length: 0\r\n";
	response += "Connection: close\r\n";
	response += "\r\n";

	return response;
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
	cgiMap = HelperClass::findLocationCgi(conf.getLocations(),"/cgi-bin/");
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
				this->responseStatus = FORBIDDEN;
				return "";
			}
			else if (isExecInt == 1)
			{
				std::string scriptContetnt;
				std::cout << ">>>> CGI YA GONDERİLDİ <<<<\n" << ext << std::endl;
				scriptContetnt = this->startCgi(newPath, ext, pollStruct,conf, cgiMap);
				path = newPath;
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
	envVec.push_back("PATH_INFO=" + this->clientRequests[fd]->getPathInfo());
	envVec.push_back("REDIRECT_STATUS=200");

	for (size_t i = 0; i < envVec.size(); i++)
	{
		env.push_back(const_cast<char *>(envVec[i].c_str()));
	}
	env.push_back(NULL);
	return env;
}
std::string WebServer::changeDir(const std::string& filePath)
{
	size_t pos = filePath.find_last_of("/");
	if (pos == std::string::npos)
		throw std::runtime_error("Invalid file path");
	std::string dirPath = filePath.substr(0, pos);
	if (chdir(dirPath.c_str()) == -1)
		throw std::runtime_error("Change directory failed");
	std::string fileName = filePath.substr(pos + 1);
	return fileName;
}
std::string WebServer::postCgi(const std::string& filePath, const std::string& cgiExecPath, std::vector<char *>& env, const std::string& requestBody)
{
    int inPipe[2];
    int outPipe[2];

    if (pipe(inPipe) == -1 || pipe(outPipe) == -1)
        throw std::runtime_error("Pipe error");

    pid_t pid = fork();
    if (pid == -1)
        throw std::runtime_error("Fork error");

    if (pid == 0) {
        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);
        close(inPipe[1]);
        close(outPipe[0]);
		std::string fileName = this->changeDir(filePath);
		char* argv[] = {
			const_cast<char*>(cgiExecPath.c_str()),  
			const_cast<char*>(fileName.c_str()),
			NULL
		};
        execve(cgiExecPath.c_str(), argv, env.data());
        perror("execve");
        exit(1);
    }

    close(inPipe[0]);
    close(outPipe[1]);

    write(inPipe[1], requestBody.c_str(), requestBody.size());
    close(inPipe[1]);

    std::string result = HelperClass::fdToString(outPipe[0]);
    close(outPipe[0]);
    waitpid(pid, NULL, 0);
    return result;
}

std::string WebServer::getCgi(const std::string& filePath, const std::string& cgiExecPath, std::vector<char *>& env)
{
	int fd[2];
	if (pipe(fd) == -1)
		throw std::runtime_error("Pipe error");
	pid_t pid = fork();
	if (pid == -1)
		throw std::runtime_error("Fork error");
	else if (pid == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		std::string fileName = this->changeDir(filePath);
		char* argv[] = {
			const_cast<char*>(cgiExecPath.c_str()),  
			const_cast<char*>(fileName.c_str()),
			NULL
		};
		execve(argv[0],argv,env.data());
		throw std::runtime_error("Execve failed");
	}
	
	close(fd[1]);
	waitpid(pid, NULL, 0);
	std::string scriptContent = HelperClass::fdToString(fd[0]);
	close(fd[0]);
	return scriptContent;
}

std::string WebServer::startCgi(const std::string&filePath, std::string& fileExt, const pollfd& pollStruct, const ServerConf& conf, const std::map<std::string,std::string>&cgiExtMap)
{
	std::string cgiExecPath = cgiExtMap.at(fileExt);
	std::string scriptContent;
	std::cout << ">>>> CGI EXEC PATH: " << cgiExecPath << "<<<<\n";
	std::cout << ">>>> CGI FILE PATH: " << filePath << "<<<<\n";
	std::cout  << "Fd: " << pollStruct.fd << std::endl;
	std::vector<char *> env = this->fillEnv(conf, pollStruct, filePath);
	if (this->clientRequests[pollStruct.fd]->getMethod() == "POST")
	{
		std::cout << ">>>> POST METHOD HANDLER CGI<<<<\n";
		return postCgi(filePath, cgiExecPath, env, this->clientRequests[pollStruct.fd]->getBody());
	}
	else if(this->clientRequests[pollStruct.fd]->getMethod() == "GET")
	{
		std::cout << ">>>> GET METHOD HANDLER  CGI <<<<\n";
		return getCgi(filePath, cgiExecPath, env);
	}
	return "";
}
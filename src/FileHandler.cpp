/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 21:40:13 by menasy            #+#    #+#             */
/*   Updated: 2025/07/04 09:02:20 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServer.hpp"

std::string WebServer::createHttpResponse(const int& pollIndex,
	const std::string& statusCode, const std::string& statusMessage,
	const std::string& contentType, const std::string& body) 
{
	std::ostringstream ss;
	std::string tmpBody, contentTypeTmp;
	std::string response = "HTTP/1.1 " + statusCode + " " + statusMessage + "\r\n";
	contentTypeTmp = contentType;
	tmpBody = body;
	if (this->isCgi)
	{
		std::string::size_type pos = body.find("\n");
		if (pos != std::string::npos)
		{
			contentTypeTmp = body.substr(0, pos);
			tmpBody = body.substr(pos + 1 , body.length() - (pos + 1));
		}	
	}
	ss << tmpBody.length();
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "Content-Type: " + contentTypeTmp + "\r\n";

	if (this->clientKeepAlive[this->pollVec[pollIndex].fd])
	{
		response += "Keep-Alive: timeout=" + HelperClass::intToString(TIMEOUT_SEC) + "\r\n";
		response += "Connection: keep-alive\r\n";
	}
	else
		response += "Connection: close\r\n"; 
	response += "\r\n";
	response += tmpBody;
	return response;
}

std::string WebServer::createErrorResponse(const int& pollIndex, const std::string& status, const ServerConf& conf, const std::string& rootPAth)
{
	std::string content, res, contentType;
	size_t pos = status.find_first_of(" ");
	std::string statusCode = status.substr(0, pos);
	std::string statusMessage = status.substr(pos + 1);
	std::map<int, std::string> errMap = conf.getErrorPages();
	std::map<int, std::string> defaultErrMap = conf.getDfltPage();
	int errCode = std::atoi(status.c_str());
	if (errMap.find(errCode) != errMap.end())
	{
		res = HelperClass::mergeDirectory(rootPAth, errMap[errCode]);
		contentType = HelperClass::findContentType(res);
		content = readHtmlFile(pollIndex,res,conf);
		if (!content.empty() )
			return createHttpResponse(pollIndex, statusCode, statusMessage, contentType, content);
	}
	return createHttpResponse(pollIndex, statusCode, statusMessage, "text/html",defaultErrMap[errCode]);
}

std::string WebServer::redirectResponse(const int& pollIndex,
										const std::string& statusCode,
										const std::string& statusMessage,
										const std::string& contentType)
{
	std::vector<LocationConf> locations = this->clientToServerMap[this->pollVec[pollIndex].fd]->getLocations();
	LocationConf* locConf = HelperClass::findLoc(this->clientRequests[this->pollVec[pollIndex].fd]->getPath(), locations);
	std::map<int, std::string> retMap = locConf->getReturn();
	int code = std::atoi(statusCode.c_str());

	std::string response = "HTTP/1.1 " + statusCode + " " + statusMessage + "\r\n";
	response += "Content-Type: " + contentType + "\r\n";

	if (retMap.find(code) != retMap.end())
		response += "Location: " + retMap[code] + "\r\n";
	else
		response += "Location: /\r\n";

	response += "Content-Length: 0\r\n";
	if (this->clientKeepAlive[this->pollVec[pollIndex].fd])
	{
		response += "Keep-Alive: timeout=" + HelperClass::intToString(TIMEOUT_SEC) + "\r\n";
		response += "Connection: keep-alive\r\n";
	}
	else
		response += "Connection: close\r\n";
	response += "\r\n";
	this->responseStatus = code;
	return response;
}

std::string WebServer::callSendResponse(const int& pollIndex, std::string status)
{
	this->sendResponse(pollIndex, status);
	return "";
}

int WebServer::fileIsExecutable(const std::string& extension, const std::map<std::string, std::string>& cgiExtMap)
{
	bool isItScript = HelperClass::isItScript(extension);
	if (!isItScript)
		return 0;
	else if ((isItScript && cgiExtMap.empty()))
		return -1;
	else
	{
		for (std::map<std::string, std::string>::const_iterator it = cgiExtMap.begin(); it != cgiExtMap.end(); it++)
		{
			if (extension == it->first)
				return 1;
		}
		return -1;
	}
	return 0;
}
std::string WebServer::pathCheck(std::string& path, std::string&rootPath,const int& pollIndex)
{
	std::string normalizedPath = path;
	size_t pos = 0;
	while ((pos = normalizedPath.find("//", pos)) != std::string::npos)
		normalizedPath.replace(pos, 2, "/");
	path = normalizedPath;
	if (path.length() > 4096)
		return this->callSendResponse(pollIndex, "414 URI Too Long");
	if (path.find('\0') != std::string::npos)
		return this->callSendResponse(pollIndex, "400 Bad Request");
	char resolved[PATH_MAX];
	if (realpath(path.c_str(), resolved) == NULL) 
		return this->callSendResponse(pollIndex, "404 Not Found");
	std::string resolvedPath = resolved;
	struct stat st;
	if (stat(resolvedPath.c_str(), &st) != 0)
		return this->callSendResponse(pollIndex, "404 Not Found");	
	if ((!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode)) || resolvedPath.find("..") != std::string::npos)
		return this->callSendResponse(pollIndex, "403 Forbidden");
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
		std::string allowedRoot = std::string(cwd) + "/" + rootPath;
		if (resolvedPath.find(allowedRoot) != 0) 
			return this->callSendResponse(pollIndex, "403 Forbidden");
	}
	const std::string dangerousExts[] = {
		".exe", ".bat", ".cmd", ".com", ".scr", ".pif", 
		".msi", ".dll", ".sh", ".bash"
	};
	size_t extCount = sizeof(dangerousExts) / sizeof(dangerousExts[0]);
	for (size_t i = 0; i < extCount; i++) {
		if (resolvedPath.length() >= dangerousExts[i].length()) {
			if (resolvedPath.substr(resolvedPath.length() - dangerousExts[i].length()) == dangerousExts[i])
				return this->callSendResponse(pollIndex, "403 Forbidden");
		}
	}
	return resolvedPath;
}

std::string WebServer::checkCgi(LocationConf* locConf, const ServerConf& conf, const int& pollIndex, std::string& newPath, int& status)
{
	std::map<std::string, std::string> cgiMap = HelperClass::findLocationCgi(locConf);
	std::size_t pos = newPath.find_last_of(".");
	if (pos != std::string::npos)
	{
		std::string ext = newPath.substr(pos, newPath.length());
		status = fileIsExecutable(ext, cgiMap);
		if (status == 1)
		{
			if (access(newPath.c_str(), X_OK) != 0)
				return this->callSendResponse(pollIndex, "403 Forbidden");
			std::string scriptContetnt;
			scriptContetnt = this->startCgi(newPath, ext, pollIndex, conf, cgiMap);
			this->isCgi = true;
			return scriptContetnt;
		}
		else if (status == -1)
			return this->callSendResponse(pollIndex, "403 Forbidden");
	}
	return "";
}

std::string WebServer::readHtmlFile(const int& pollIndex,std::string& path, const ServerConf& conf) 
{
	if (path.empty())
		return "";
	if (path == "errorPage")
		return conf.getDfltPage().at(404);
	std::string newPath;
	int status = 0;	
	std::string locPath = this->clientRequests[this->pollVec[pollIndex].fd]->getPath();
	std::vector<LocationConf> locVec = conf.getLocations();
	LocationConf* locConf = HelperClass::findLoc(locPath, locVec);
	newPath = HelperClass::checkFileWithExtension(path);
    std::ifstream file(newPath.c_str());	
	if (file.is_open()) 
	{
		std::string retValCgi = checkCgi(locConf, conf, pollIndex, newPath, status);
		if (status != 0)
		{
			file.close();
			return retValCgi;
		}
    }
	else
		return "";
	if (access(newPath.c_str(), R_OK) != 0)
		return this->callSendResponse(pollIndex, "403 Forbidden");
    std::stringstream buffer;
    buffer << file.rdbuf();
	file.close();
	path = newPath;
	return buffer.str();
}

std::vector<char *> WebServer::fillEnv(const ServerConf& conf, const int& pollIndex, const std::string& path)
{
	std::vector<char *> env;
	std::vector<std::string> envVec;
	const int& fd = this->pollVec[pollIndex].fd;
	envVec.push_back("REQUEST_METHOD=" + this->clientRequests[fd]->getMethod());
	envVec.push_back("SCRIPT_FILENAME=" + path);
	envVec.push_back("SCRIPT_NAME=" + this->clientRequests[fd]->getPath() + this->clientRequests[fd]->getRequestFile());
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
	for (size_t i = 0; i < envVec.size(); i++) {
    char* var = new char[envVec[i].size() + 1]; 
		std::strncpy(var, envVec[i].c_str(), envVec[i].size());
		var[envVec[i].size()] = '\0';
		env.push_back(var);
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

std::string WebServer::startCgi(const std::string&filePath, std::string& fileExt, const int& pollIndex, const ServerConf& conf, const std::map<std::string,std::string>&cgiExtMap)
{
	std::string cgiExecPath = cgiExtMap.at(fileExt);
	std::string scriptContent;
	std::vector<char *> env = this->fillEnv(conf, pollIndex, filePath);
	if (this->clientRequests[this->pollVec[pollIndex].fd]->getMethod() == "POST")
		scriptContent = postCgi(filePath, cgiExecPath, env, this->clientRequests[this->pollVec[pollIndex].fd]->getBody());
	else if(this->clientRequests[this->pollVec[pollIndex].fd]->getMethod() == "GET")
		scriptContent = getCgi(filePath, cgiExecPath, env);
	else
		scriptContent = "";
	HelperClass::freeEnv(env);
	return scriptContent;
}
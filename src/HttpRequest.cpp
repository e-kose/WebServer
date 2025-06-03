/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:36:39 by menasy            #+#    #+#             */
/*   Updated: 2025/06/03 18:50:08 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpRequest.hpp"
#include "../include/ServerConf.hpp"
#include "../include/LocationConf.hpp"


HttpRequest::HttpRequest() {
	
	this->method = "";
	this->path = "";
	this->version = "";
	this->contentLength = 0;
	this->headers.clear();
	this->body = "";
	this->queryString = "";
	this->queryParams.clear();
	this->bodyMap.clear();
	this->hostName = "";
	
}
HttpRequest::HttpRequest(const HttpRequest &other) {
	*this = other;
}
HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
	if (this != &other) {
		this->method = other.method;
		this->path = other.path;
		this->version = other.version;
		this->contentLength = other.contentLength;
		this->headers = other.headers;
		this->body = other.body;
		this->queryString = other.queryString;
		this->queryParams = other.queryParams;
		this->bodyMap = other.bodyMap;
		this->hostName = other.hostName;
	}
	return *this;
}
HttpRequest::~HttpRequest() {}

std::string HttpRequest::getMethod() const {
	return method;
}
std::string HttpRequest::getPath() const {
	return path;
}
std::string HttpRequest::getVersion() const {
	return version;
}
size_t HttpRequest::getContentLength() const {
	return contentLength;
}
std::map<std::string, std::string> HttpRequest::getHeaders() const {
	return headers;
}
std::string HttpRequest::getBody() const {
	return body;
}

std::string HttpRequest::getHostName() const {
	return hostName;
}
std::string HttpRequest::getRequestFile() const {
	return requestFile;
}
std::string HttpRequest::getContentType() const {
	return contentType;
}
std::string HttpRequest::getQueryString() const {
	return queryString;
}
std::map<std::string, std::string> HttpRequest::getBodyMap() const {
	return this->bodyMap;
}
std::map<std::string, std::string> HttpRequest::getQueryParams() const {
	return queryParams;
}

void HttpRequest::setHeaders(const std::map<std::string, std::string>  headers) {
	this->headers = headers;
}

void HttpRequest::setMethod(const std::string method) {
	this->method = method;
}

void HttpRequest::setRequestFile(const std::string requestFile) {
	this->requestFile = requestFile;
}
void HttpRequest::setContentType(const std::string contentType) {
	this->contentType = contentType;
}



void HttpRequest::setPath(const std::string path) 
{
	std::cout << "GELEN PATH:" << path << std::endl;
	std::string tmpPath = path;
	size_t pos = tmpPath.find_first_of('?');
	if (pos != std::string::npos)
	{
		std::string queryStr = tmpPath.substr(pos + 1);
		this->setQueryString(queryStr); // env için koydum
		this->parseQuery(queryStr);
		tmpPath = tmpPath.substr(0, pos);
	}
	this->path = tmpPath;
}

void HttpRequest::sepPath(const std::vector<LocationConf>& locVec)
{
	if (this->path == "/")
		return;
	std::string tmpPath = this->path;
	this->path = HelperClass::getLocInVec(tmpPath, locVec);
	this->setRequestFile(tmpPath.substr(this->path.length(), tmpPath.length()));
	std::cout << "SEPERATED PATH___________:" << this->path <<std::endl;
	std::cout << "REQ FİLE___________:" << this->requestFile << std::endl;
}

void HttpRequest::setVersion(const std::string version) {
	this->version = version;
}

void HttpRequest::setHostName(const std::string hostName) {
	std::string::size_type pos = hostName.find_first_of(':');
	if (pos != std::string::npos)
		this->hostName = hostName.substr(0, pos);
	else
		this->hostName = hostName;
}
void HttpRequest::setContentLength(size_t length) {
	this->contentLength = length;
}


void	HttpRequest::addQuery(std::string key, std::string value)
{
	this->queryParams[key] = value;
}
void HttpRequest::setBody(const std::string body) {
	this->body = body;
}

void HttpRequest::setBodyMap(const std::map<std::string, std::string>  bodyMap) {
	this->bodyMap = bodyMap;
}
void HttpRequest::setQueryParams(const std::map<std::string, std::string> queryParams) {
	this->queryParams = queryParams;
}

void HttpRequest::setQueryString(const std::string queryString) {
	this->queryString = queryString;
}
std::map<std::string, std::string>  HttpRequest::parseHeader(std::string& parseStr)
{
	std::map<std::string, std::string> destMap;
	std::string line;
	std::string::size_type pos;
	std::istringstream strStream(parseStr);
	while (std::getline(strStream, line)) 
	{
		if (line.find_first_of("{}") != std::string::npos || line.empty())
			break;
		pos = line.find_first_of(":");
		if (pos == std::string::npos)
			break;
		std::string key = HelperClass::trimLine(line.substr(0, pos));
		std::string value = HelperClass::trimLine(line.substr(pos + 1, line.length() - pos));
		if (key.empty() || value.empty())
			throw std::runtime_error("Key or value is empty"); // Bnu geçici kodum kaldırılabilir.
		destMap[key] = value;
	}
	this->setHostName(destMap["Host"]);
	this->setContentType(destMap["Content-Type"]);
	if(this->getContentType() == "application/x-www-form-urlencoded")
	{
		while (std::getline(strStream, line))
		{
			if (line.find_first_of('=') != std::string::npos)
				this->parseQuery(line);
		}
	}
	return destMap;
}
std::map<std::string, std::string>  HttpRequest::parseBody()
{
	std::map<std::string, std::string> destMap;
	std::string key;
	std::string value;
	size_t pos;
	if (this->body.empty())
		return destMap;
	pos = this->body.find_first_of("\"");
	for (size_t i = 0; i < this->body.length(); i++)
	{
		if (this->body[i] == ':' && HelperClass::characterCounter(this->body.substr(pos,i - pos), '\"') % 2 == 0)
		{
			key = HelperClass::trimWithCharacter(this->body.substr(pos, i - pos),"\"\",\t\n ");
			pos = i + 1;
			while (this->body[i] != '\0' && this->body[i] != ',')
				i++;
			value =  HelperClass::trimWithCharacter(this->body.substr(pos, i - pos),"\"\",\t\n ");
			pos = i + 1;
			destMap[key] = value;
		}
	}
	return destMap;
}
void HttpRequest::parseRequest(const std::string& request) 
{
	std:: cout << "================== PARSEEEE REQUEST ================== \n" << std::endl;
	std::string tmpReq = request;
	this->setMethod(HelperClass::createAndMove(tmpReq," "));
	this->setPath(HelperClass::createAndMove(tmpReq," "));
	this->setVersion(HelperClass::createAndMove(tmpReq,"\n"));
	this->setHeaders(this->parseHeader(tmpReq));
	if (tmpReq.find_first_of("{") == std::string::npos && !tmpReq.empty())
	{
		this->setBody("");
		return ;
	}
	this->setBody(tmpReq.substr(tmpReq.find_first_of("{"), tmpReq.find_last_of("}") - tmpReq.find_first_of("{") + 1));
	this->setBodyMap(this->parseBody());

}

void	HttpRequest::parseQuery(std::string query)
{
	std::istringstream stream(query);
    std::string pair;

    while (std::getline(stream, pair, '&'))
	{
        size_t pos = pair.find('=');
        if (pos != std::string::npos)
		{
            std::string key = pair.substr(0, pos);
            std::string value = pair.substr(pos + 1);
            this->addQuery(key, value);
        }
    }
}

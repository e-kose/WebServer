/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:36:39 by menasy            #+#    #+#             */
/*   Updated: 2025/05/10 18:36:10 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpRequest.hpp"


HttpRequest::HttpRequest() {
	
	this->method = "";
	this->path = "";
	this->version = "";
	this->contentLength = 0;
	this->headers.clear();
	this->body = "";
	this->queryString = "";
	this->queryParams.clear();
	this->bodyVec.clear();
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
		this->bodyVec = other.bodyVec;
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
std::map<std::string, std::string> HttpRequest::getBodyVec() const {
	return bodyVec;
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

void HttpRequest::setPath(const std::string path) 
{
	this->path = HelperClass::trimLine(path);
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


void HttpRequest::setBody(const std::string body) {
	this->body = body;
}

void HttpRequest::setBodyVec(const std::map<std::string, std::string>  bodyVec) {
	this->bodyVec = bodyVec;
}
void HttpRequest::setQueryParams(const std::map<std::string, std::string> queryParams) {
	this->queryParams = queryParams;
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
	std::string tmpReq = request;
	
	this->setMethod(HelperClass::createAndMove(tmpReq," "));
	if (HelperClass::characterCounter(request.substr(0,request.find_first_of('\n')),' ') == 2)
	{
		this->setPath(HelperClass::createAndMove(tmpReq," "));
		this->setVersion(HelperClass::createAndMove(tmpReq,"\n"));
	}
	else 
		this->setVersion(HelperClass::createAndMove(tmpReq,"\n"));
	this->setHeaders(this->parseHeader(tmpReq));
	if (tmpReq.find_first_of("{") == std::string::npos && !tmpReq.empty())
	{
		this->setBody("");
		return ;
	}
	this->setBody(tmpReq.substr(tmpReq.find_first_of("{"), tmpReq.find_last_of("}") - tmpReq.find_first_of("{")));
	this->setBodyVec(this->parseBody());

}


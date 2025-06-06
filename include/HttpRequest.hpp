/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:17:07 by menasy            #+#    #+#             */
/*   Updated: 2025/05/08 16:17:36 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

class ServerConf;
class LocationConf;
class HelperClass;
class HttpRequest 
{
	private:
		std::string method;								
		std::string path;   							
		std::string version;							
		std::string hostName;
		std::string requestFile;
		std::string contentType;	
		int port;									
		std::map<std::string, std::string>headers; 	// Tüm HTTP headerları
		std::map<std::string, std::string>bodyMap;
		std::string body;								
		std::string queryString;						
		size_t contentLength;						
		std::map<std::string, std::string> queryParams;
		std::string pathInfo;
	public:
		HttpRequest();
		HttpRequest(const HttpRequest &other);
		HttpRequest &operator=(const HttpRequest &other);
		~HttpRequest();
		
		void parseRequest(const std::string& request);
		std::map<std::string, std::string> parseHeader(std::string& parseStr);
		std::map<std::string, std::string> parseBody();
		void								parseQuery(std::string query);
		
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getHostName() const;
		size_t getContentLength() const;
		std::string getRequestFile() const;
		std::string getContentType() const;
		std::string getQueryString() const;
		int getPort() const;
		std::map<std::string, std::string>getHeaders() const;
		std::string getBody() const;
		std::map<std::string, std::string> getBodyMap() const;
		std::map<std::string, std::string> getQueryParams() const;
		std::string getPathInfo() const;

		void setMethod(std::string method);
		void setPath(std::string path);
		void setPort(int port);
		void setVersion(std::string version);
		void setHostName(std::string hostName);
		void setContentType(std::string contentType);
		void setRequestFile(std::string requestFile);
		void setContentLength(size_t length);
		void setHeaders( std::map<std::string, std::string> headers);
		void setBody(std::string body);
		void setBodyMap(std::map<std::string, std::string> bodyMap);
		void setQueryParams(std::map<std::string, std::string> queryParams);
		void setQueryString(std::string queryString);
		void addQuery(std::string key, std::string value);
		void sepPath(ServerConf conf);
		void setPathInfo(std::string pathInfo, ServerConf& conf);
};
	

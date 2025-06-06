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
		std::string method;								// GET, POST, etc.
		std::string path;   							// /users/5
		std::string version;							// HTTP/1.1
		std::string hostName;
		std::string requestFile;
		std::string contentType;
		int port;									// Port numarası
		std::map<std::string, std::string>headers; 	// Tüm HTTP headerları
		std::map<std::string, std::string>bodyMap;

		std::string body;								// içerik kısmı json falan 
		std::string queryString;						// /search?q=test ise q=test bu olacak mı bilmiyroum daha
		size_t contentLength;						// Header'dan alcaz
		std::map<std::string, std::string> queryParams; // query parametreleri ayrıştırılmış hali	 en son bakacam	
		bool                               chunkedTransfer;
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
		bool getChunkedTransfer() const;

		void setMethod(const std::string method);
		void setPath(const std::string path);
		void setPort(int port);
		void setVersion(const std::string version);
		void setHostName(const std::string hostName);
		void setContentType(const std::string contentType);
		void setRequestFile(const std::string requestFile);
		void setContentLength(size_t length);
		void setHeaders(const  std::map<std::string, std::string> headers);
		void setBody(const std::string body);
		void setBodyMap(const std::map<std::string, std::string> bodyMap);
		void setQueryParams(const std::map<std::string, std::string> queryParams);
		void setQueryString(const std::string queryString);
		void addQuery(std::string key, std::string value);
		void sepPath(const std::vector<LocationConf>&);
		void setChunkedTransfer(bool chunked);
};
	

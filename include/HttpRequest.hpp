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
#include "HelperClass.hpp"

class HttpRequest 
{
	private:
		std::string method;								// GET, POST, etc.
		std::string path;   							// /users/5
		std::string version;							// HTTP/1.1
		std::string hostName;
		std::string requestFile;
		std::map<std::string, std::string>headers; 	// Tüm HTTP headerları
		std::map<std::string, std::string>bodyVec;

		std::string body;								// içerik kısmı json falan 
		std::string queryString;						// /search?q=test ise q=test bu olacak mı bilmiyroum daha
		size_t contentLength;						// Header'dan alcaz
		std::map<std::string, std::string> queryParams; // query parametreleri ayrıştırılmış hali	 en son bakacam	
	public:
		HttpRequest();
		HttpRequest(const HttpRequest &other);
		HttpRequest &operator=(const HttpRequest &other);
		~HttpRequest();
		
		void parseRequest(const std::string& request);
		std::map<std::string, std::string> parseHeader(std::string& parseStr);
		std::map<std::string, std::string> parseBody();

		
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getHostName() const;
		size_t getContentLength() const;
		std::string getRequestFile() const;
		std::map<std::string, std::string>getHeaders() const;
		std::string getBody() const;
		std::map<std::string, std::string> getBodyVec() const;
		std::map<std::string, std::string> getQueryParams() const;

		void setMethod(const std::string method);
		void setPath(const std::string path);
		void setVersion(const std::string version);
		void setHostName(const std::string hostName);
		void setRequestFile(const std::string requestFile);
		void setContentLength(size_t length);
		void setHeaders(const  std::map<std::string, std::string> headers);
		void setBody(const std::string body);
		void setBodyVec(const std::map<std::string, std::string> bodyVec);
		void setQueryParams(const std::map<std::string, std::string> queryParams);
		void setQueryString(const std::string queryString);
		
};
	

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:40:04 by menasy            #+#    #+#             */
/*   Updated: 2025/05/29 19:08:15 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iterator>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ServerConf.hpp"
#include "HelperClass.hpp"
#include "HttpRequest.hpp"
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <cstdio>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include <sys/stat.h>
#include <limits.h>
#include <sys/wait.h>

class WebServer 
{
	// TUM HER ŞEY BİTTİKTEN SONRA COPY CONSTRUCTORLAR FALAN KONTROL ET DEĞERLERİ ATA !!!!!!!!
	private:
		WebServer();
		std::vector<pollfd>						pollVec;
		std::vector<ServerConf>&				serverConfVec;
		std::map<int, std::vector<ServerConf> >	socketMap;
		std::map<int, ServerConf*>				clientToServerMap;
		std::map<int, struct sockaddr_in>		clientToAddrMap;
		std::map<int, HttpRequest*>     		clientRequests;
		std::map<int, std::string>				requestBuffers;
		std::string 							resultPath;
		std::string 							response;

		
		void									pollfdVecCreat();
		std::string								socketInfo(sockaddr_in&, int);
		void									closeCliSocket(int);
		void 									pollOutEvent(pollfd& pollStruct);
		void									deleteMethod(pollfd&);
		bool									indexHandler(std::string& mergedPath, const std::vector<std::string>& indexVec);
		void									acceptNewClient(pollfd& pollStruct);
		void									clientRead(pollfd&);
		void									setNonBlocking(int);
		void									initSocket();
		bool									isExistIpAndPort(ServerConf&);
		void									runServer();
		HttpRequest*							parseRecv(const std::string&);
		ServerConf&								searchServerConf(std::vector<ServerConf>& , std::string);			
		std::string 							findRequest(pollfd& pollStruct);
		void 									tryFiles(LocationConf& locConf, const std::string& httpPath, const ServerConf* serverConfMap,  pollfd& pollStruct);
		bool 									methodIsExist(const std::vector<std::string>& locMethodsvec, const std::string& requestMethod, pollfd&);
		void									sendHandler(pollfd& pollStruct, std::string& sendMessage);
		void 									sendResponse(pollfd&, const std::string& status);
		std::string 							readHtmlFile(pollfd& ,std::string& path, const ServerConf& conf); 
		std::string 							createErrorResponse(pollfd& pollStruct,const std::string& status, const ServerConf& conf, const std::string& rootPAth);
		std::string 							createHttpResponse(
																const std::string& statusCode, const std::string& statusMessage,
																const std::string& contentType, const std::string& body);
		std::map<std::string, std::string>	findLocation(const ServerConf& conf, std::string locStr);
		std::string getCgi(const std::string& filePath, const std::string& cgiExecPath, std::vector<char *>& env);
		std::vector<char *>	fillEnv(const ServerConf& conf, const pollfd& pollStruct, const std::string& path);
		std::string startCgi(const std::string&filePath, std::string& fileExt, const pollfd& pollStruct, const ServerConf& conf, const std::map<std::string,std::string>&cgiExtMap);
		std::string postCgi(const std::string& filePath, const std::string& cgiExecPath, std::vector<char *>& env, const std::string& requestBody);

		public:
			WebServer(std::vector<ServerConf>& serverConfVec);
			WebServer(const WebServer &other);
			WebServer &operator=(const WebServer &other);
			~WebServer();
		
	
};
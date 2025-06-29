/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:40:04 by menasy            #+#    #+#             */
/*   Updated: 2025/06/29 11:33:44 by ekose            ###   ########.fr       */
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
#include <filesystem>

#define NOT_FOUND 404
#define OK 200
#define FORBIDDEN 403
#define METHOD_NOT_ALLOWED 405
#define NOT_RESPONDED 0
#define PAYLOAD_TOO_LARGE 413
#define BAD_REQUEST 400
#define TIMEOUT_SEC 5

class WebServer 
{
	// TUM HER ŞEY BİTTİKTEN SONRA COPY CONSTRUCTORLAR FALAN KONTROL ET DEĞERLERİ ATA !!!!!!!!
	private:
		WebServer();
		std::vector<pollfd>						pollVec;
		std::vector<ServerConf>&				serverConfVec;
		std::map<int, std::vector<ServerConf> >	socketMap;
		std::map<int, ServerConf*>				clientToServerMap;
		std::map<int, time_t> 					lastActivity;
		std::map<int, struct sockaddr_in>		clientToAddrMap;
		std::map<int, HttpRequest*>     		clientRequests;
		std::map<int, std::string>				requestBuffers;
		std::map<int, bool>						clientKeepAlive; 
		std::map<int, bool>						headerIsParsed;

		
		std::string								unchunkedBody;
		std::string 							requestHeader;
		std::string								requestBody;	
		std::string 							resultPath;
		std::string 							response;
		
		int 									responseStatus;

		int										fileIsExecutable(const std::string& extension, const std::map<std::string, std::string>& cgiExtMap);
		ServerConf&								searchServerConf(std::vector<ServerConf>& , std::string);			
		std::vector<char *>						fillEnv(const ServerConf& conf, const pollfd& pollStruct, const std::string& path);
		HttpRequest*							parseRecv(const std::string&);
		
		
		bool 									methodIsExist(LocationConf* locConf, const std::string& requestMethod, pollfd&);
		bool									indexHandler(std::string& mergedPath, const std::vector<std::string>& indexVec);
		bool									headerHandle(pollfd& pollStruct);
		bool									isExistIpAndPort(ServerConf&);
		bool									clientRead(pollfd&);
		bool 									checkTimeouts();
		
		std::string								tryFiles(std::string tryPath,LocationConf* locConf, const ServerConf* serverConfMap, 
														pollfd& pollStruct, std::vector<LocationConf>& locVec);
		std::string								checkCgi(LocationConf* locConf, const ServerConf& conf, pollfd& pollStruct, 
														std::string& newPath, int& status);
		std::string 							createErrorResponse(pollfd& pollStruct,const std::string& status, const ServerConf& conf, const std::string& rootPAth);
		std::string 							createHttpResponse(pollfd& pollStruct,
																const std::string& statusCode, const std::string& statusMessage,
																const std::string& contentType, const std::string& body);
		std::string								pathCheck(std::string& path, std::string& rootPath, pollfd& pollStruct);
		std::string 							readHtmlFile(pollfd& ,std::string& path, const ServerConf& conf); 
		std::string								callSendResponse(pollfd& polstruct, std::string status);
		std::string 							findRequest(pollfd& pollStruct);
		std::string								socketInfo(sockaddr_in&, int);
		std::string 							getCgi(const std::string& filePath, const std::string& cgiExecPath, std::vector<char *>& env);
		std::string 							startCgi(const std::string&filePath, std::string& fileExt, const pollfd& pollStruct, const ServerConf& conf, const std::map<std::string,std::string>&cgiExtMap);
		std::string 							postCgi(const std::string& filePath, const std::string& cgiExecPath, std::vector<char *>& env, const std::string& requestBody);
		std::string 							mergedPathHandler(std::string& mergedPath, LocationConf *locConf, const ServerConf& serverConf, pollfd& pollStruct, int mergedPathIndex);
		std::string 							changeDir(const std::string& filePath);
		std::string 							redirectResponse(pollfd& poolStruct,const std::string& statusCode,
																const std::string& statusMessage, const std::string& contentType);
		
		void									pollfdVecCreat();
		void									closeCliSocket(int);
		void 									pollOutEvent(pollfd& pollStruct);
		void									deleteMethod(pollfd&);
		void									acceptNewClient(pollfd& pollStruct);
		void									setNonBlocking(int);
		void 									sendResponse(pollfd&, const std::string& status);
		void									initSocket();
		void									runServer();
		void									sendHandler(pollfd& pollStruct, std::string& sendMessage);
		void 									listDirectory(const std::string& path,LocationConf* locConf, pollfd& pollStruct);		
		void									cleanReq(pollfd& pollStruct);
		
		public:
			WebServer(std::vector<ServerConf>& serverConfVec);
			WebServer(const WebServer &other);
			WebServer &operator=(const WebServer &other);
			~WebServer();
			const std::map<int, ServerConf*>	getServerConfForFdMap()const;

		
	
};
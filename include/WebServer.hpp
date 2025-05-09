/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:40:04 by menasy            #+#    #+#             */
/*   Updated: 2025/05/09 16:58:24 by menasy           ###   ########.fr       */
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

class WebServer {
	private:
		WebServer();
		std::vector<pollfd>				pollVec;
		std::vector<ServerConf>&		serverConfVec;
		std::map<int, ServerConf>		socketMap;
		std::map<int, ServerConf*>		clientToServerMap;
		void							pollfdVecCreat();
		HttpRequest*					pollInEvent(pollfd&);
		std::string						socketInfo(int);
		void							closeCliSocket(int);
		void 							pollOutEvent(pollfd& pollStruct, HttpRequest* httpRequest);

	public:
		WebServer(std::vector<ServerConf>& serverConfVec);
		WebServer(const WebServer &other);
		WebServer &operator=(const WebServer &other);
		~WebServer();
		
		void	setNonBlocking(int);
		void	initSocket();
		bool	isExistIpAndPort(const std::string&, int);
		void	runServer();
		HttpRequest* parseRecv(const std::string& request);

};
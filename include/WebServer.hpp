/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:40:04 by menasy            #+#    #+#             */
/*   Updated: 2025/05/03 16:33:58 by ekose            ###   ########.fr       */
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
#include "unistd.h"
#include <fcntl.h>
#include <poll.h>

class WebServer {
	private:
		std::vector<ServerConf>			serverConfVec;
		std::map<int, ServerConf>		socketMap;
		std::map<int, ServerConf*>		clientToServerMap;
	public:
		WebServer();
		WebServer(std::vector<ServerConf>& serverConfVec);
		WebServer(const WebServer &other);
		WebServer &operator=(const WebServer &other);
		~WebServer();
		
		void	setNonBlocking(int);
		void	initSocket();
		bool	isExistIpAndPort(const std::string&, int);
		void	runServer();
};
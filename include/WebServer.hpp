/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:40:04 by menasy            #+#    #+#             */
/*   Updated: 2025/05/02 16:25:18 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iterator>
# include "ServerConf.hpp"
class WebServer {
	private:
		std::vector<ServerConf> serverConfVec;
		std::map<int,ServerConf> socketMap;
	public:
		WebServer();
		WebServer(std::vector<ServerConf>& serverConfVec);
		WebServer(const WebServer &other);
		WebServer &operator=(const WebServer &other);
		~WebServer();
		
		void initSocket();
		bool isExistIpAndPort(const std::string& ip, int& port);
		
		

};
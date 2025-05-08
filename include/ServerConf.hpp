#pragma once

#include <iostream>
#include <vector>
#include "../include/LocationConf.hpp"
#include "../include/HelperClass.hpp"
#include <map>
#include <string>

class LocationConf;
class ServerConf {
	private:
		int							port;
		size_t						body_size;
		std::string					ip;
		std::string 				root;
		std::string					error_log;
		std::string					access_log;
		std::vector<std::string>	index;
		std::map<int, std::string>	error_pages;
		std::vector<LocationConf>	locations;
		std::vector<std::string>	server_name;
	public:
		//Constructor
		ServerConf();
		~ServerConf();
		ServerConf(const ServerConf &other);
		ServerConf &operator=(const ServerConf &other);
		
		
		// GET Functions
		int getPort() const;
		size_t						getBodySize() const;
		std::string					getIp() const;
		std::string					getRoot() const;
		std::string					getErrorLog()const;
		std::string					getAccessLog()const;
		std::vector<std::string>	getIndex() const;
		std::vector<std::string>	getServerName() const;
		std::vector<LocationConf>	getLocations() const;
		std::map<int, std::string>	getErrorPages() const;

		// SET Functions
		void						setIp(std::string ip);
		void						setPort(int port);
		void						setRoot(std::string root);
		void						setServerName(std::vector<std::string>& server_name);
		void						setIndex(std::vector<std::string>);
		void						setBodySize(size_t body_size);
		void						setErrorPages(std::map<int, std::string>& error_pages);
		void						setLocations(std::vector<LocationConf>& locations);
		void						setErrorLog(std::string);
		void						setAccesLog(std::string);
		void						addLocation(LocationConf& location);
		void						addErrorPage(int code, std::string page);
		void						addServerName(std::string name);
		void						addIndex(std::string);		

		
};
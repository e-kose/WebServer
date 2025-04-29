#pragma once

#include <iostream>
#include <vector>
#include "../include/LocationConf.hpp"
#include <map>
#include <string>

class LocationConf;
class ServerConf {
	private:
		std::string					ip;
		int							port;
		std::string 				root;
		std::vector<std::string>	server_name;
		std::string					index;
		size_t						body_size;
		std::map<int, std::string>	error_pages;
		std::vector<LocationConf>	locations;
	public:
		ServerConf();
		~ServerConf();
		ServerConf(const ServerConf &other);
		ServerConf &operator=(const ServerConf &other);
		
		std::string getIp() const;
		int getPort() const;
		std::string getRoot() const;
		std::vector<std::string> getServerName() const;
		std::string getIndex() const;
		size_t getBodySize() const;
		std::map<int, std::string> getErrorPages() const;
		std::vector<LocationConf> getLocations() const;

		void setIp(std::string ip);
		void setPort(int port);
		void setRoot(std::string root);
		void setServerName(std::vector<std::string>& server_name);
		void setIndex(std::string index);
		void setBodySize(size_t body_size);
		void setErrorPages(std::map<int, std::string>& error_pages);
		void setLocations(std::vector<LocationConf>& locations);
		void addLocation(LocationConf& location);
		void addErrorPage(int code, std::string page);
};
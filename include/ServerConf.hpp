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
		std::map<int, std::string>		dflt_page;
		std::map<int, std::string>		error_pages;
		std::vector<LocationConf>		locations;
		std::vector<std::string>		server_name;
		std::vector<std::string>		index;
		std::string						error_log;
		std::string						access_log;
		std::string 					root;
		std::string						ip;
		size_t							body_size;
		int								port;

		void							defaultPage();
	public:
		
		ServerConf();
		~ServerConf();
		ServerConf(const ServerConf &other);
		ServerConf &operator=(const ServerConf &other);
		
		
		int 							getPort() const;

		size_t							getBodySize() const;

		std::string						getIp() const;
		std::string						getRoot() const;
		std::string						getErrorLog()const;
		std::string						getAccessLog()const;

		std::vector<std::string>		getIndex() const;
		std::vector<std::string>		getServerName() const;
		std::vector<LocationConf>		getLocations() const;
		std::map<int, std::string>		getErrorPages() const;
		std::map<int, std::string>		getDfltPage() const;

		void							setIp(std::string ip);
		void							setPort(int port);
		void							setRoot(std::string root);
		void							setBodySize(size_t body_size);
		void							setLocations(std::vector<LocationConf>& locations);
		void							setErrorLog(std::string);
		void							setAccesLog(std::string);
		void							addLocation(LocationConf location);
		void							addErrorPage(int code, std::string page);
		void							addServerName(std::string name);
		void							addIndex(std::string);		
};
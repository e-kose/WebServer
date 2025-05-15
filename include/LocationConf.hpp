#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "../include/HelperClass.hpp"

class LocationConf{
	private:
		bool						autoIndex;
		std::string					path;
		std::string					upload_store;
		std::string					root;
		std::vector<std::string>	methods;
		std::vector<std::string>	try_files;
		std::vector<std::string>	index;
		std::map<int, std::string>	return_;
		std::map<std::string, std::string>	cgi_ext;
	public:
		//Constructor
		LocationConf();
		LocationConf(const LocationConf& src);
		LocationConf& operator=(const LocationConf& target);
		~LocationConf();

		// GET Functions
		bool								getAutoIndex() const;
		std::string							getPath() const;
		std::vector<std::string>			getMethods() const;
		std::string 						getUploadStore() const;
		std::string							getRoot() const;
		std::vector<std::string>			getTryFiles()const;
		std::vector<std::string>			getIndex()const;
		std::map<int, std::string>			getReturn() const;
		std::map<std::string, std::string>	getCgiExt() const;
		
		// SET Functions
		void						setPath(std::string);
		void						setMethods(std::vector<std::string>);
		void						setAutoIndex(bool);
		void						setUploadStore(std::string);
		void						setRoot(std::string);
		void						setTryFiles(std::vector<std::string>);
		void						setIndex(std::vector<std::string>);
		void						setReturn(std::map<int, std::string>);
		void						addTryFiles(std::string);
		void						addMethod(std::string);
		void						addIndex(std::string);
		void						addReturn(int, std::string);
		void						addCgiExt(std::string, std::string);
};

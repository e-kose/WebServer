#pragma once

#include <iostream>
#include <vector>
#include <map>

class LocationConf{
	private:
		std::string					path;
		std::vector<std::string>	methods;
		bool						autoIndex;
		std::string					upload_store;
		std::string					root;
		std::string 				cgi_extension;
		std::string 				cgi_path;
	public:
		LocationConf();
		LocationConf(const LocationConf& src);
		LocationConf& operator=(const LocationConf& target);
		~LocationConf();
		std::string getPath() const;
		std::vector<std::string> getMethods() const;
		bool getAutoIndex() const;
		std::string getUploadStore() const;
		std::string getRoot() const;
		std::string getCgiExtension() const;
		std::string getCgiPath() const;
		void setPath(std::string path);
		void setMethods(std::vector<std::string> methods);
		void setAutoIndex(bool autoIndex);
		void setUploadStore(std::string upload_store);
		void setRoot(std::string root);
		void setCgiExtension(std::string cgi_extension);	
		void setCgiPath(std::string cgi_path);
};

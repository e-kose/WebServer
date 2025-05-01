#include <iostream>
#include <fstream>
#include <sstream>
#include "include/ServerConf.hpp"
#include "include/Tokenizer.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " <config_file>" << std::endl;
		return 1;
	}

	std::ifstream file("config/conf.conf");  // Dosyayı aç
    if (!file) {  // Dosya açılmadıysa hata mesajı ver
        std::cerr << "Dosya açılamadı!" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // Dosyadaki tüm içeriği stringstream'e aktar

    std::string fileContent = buffer.str(); 
	Tokenizer *tok = new Tokenizer(fileContent);
	std::vector<std::string> v = tok->seperation();
	std::vector<ServerConf> conf = tok->createConfVec(v);
	std::vector<ServerConf>::iterator it = conf.begin();
	for (; it != conf.end() ; it++)
	{
		std::cout << "Server IP: " << it->getIp() << std::endl;
		std::cout << "Server Port: " << it->getPort() << std::endl;
		std::cout << "Server Root: " << it->getRoot() << std::endl;
		std::cout << "Server Name: ";
		std::vector<std::string> servername = it->getServerName();
		std::vector<std::string>::iterator it2 = servername.begin();
		for (; it2 != servername.end() ; it2++)
			std::cout << *it2 << " ";
		std::cout << std::endl;
		std::cout << "Server Index: ";
		std::vector<std::string> index = it->getIndex();
		std::vector<std::string>::iterator it3 = index.begin();
		for (; it3 != index.end() ; it3++)
			std::cout << *it3 << " ";
		std::cout << std::endl;
		std::cout << "Server Body Size: " << it->getBodySize() << std::endl;
		std::cout << "Server Error Log: " << it->getErrorLog() << std::endl;
		std::cout << "Server Access Log: " << it->getAccesLog() << std::endl;
		std::cout << "Server Error Pages: ";
		std::map<int, std::string> erorpages = it->getErrorPages();
		std::map<int, std::string>::iterator end = erorpages.end();
		std::map<int, std::string>::iterator it4 = erorpages.begin();
		for (; it4 != end; it4++)
			std::cout << it4->first << " " << it4->second << " ";
		std::cout << std::endl;
		std::cout << "Server Locations: ";
		std::vector<LocationConf> locations = it->getLocations();
		int i = 0;
		for (; i < locations.size() ; i++)
		{
			std::cout << "Location Path: " << locations[i].getPath() << std::endl;
			std::cout << "Location AutoIndex: " << locations[i].getAutoIndex() << std::endl;
			std::cout << "Location Root: " << locations[i].getRoot() << std::endl;
			std::cout << "Location Cgi Extension: " << locations[i].getCgiExtension() << std::endl;
			std::cout << "Location Cgi Path: " << locations[i].getCgiPath() << std::endl;
			std::cout << "Location Cgi Pass: " << locations[i].getCgiPass() << std::endl;
			std::cout << "Location Upload Store: " << locations[i].getUploadStore() << std::endl;
			std::cout << "Location Methods: ";
			std::vector<std::string> methods = locations[i].getMethods();
			std::vector<std::string>::iterator it6 = methods.begin();
			for (; it6 != methods.end() ; it6++)
				std::cout << *it6 << " ";
			std::cout << std::endl;
			std::cout << "Location Try Files: ";
			std::vector<std::string> tryfiles = locations[i].getTryFiles();
			std::vector<std::string>::iterator it7 = tryfiles.begin();
			for (; it7 != tryfiles.end() ; it7++)
				std::cout << *it7 << " ";
			std::cout << std::endl;
			std::cout << "Location Index: ";
			std::vector<std::string> index = locations[i].getIndex();
			std::vector<std::string>::iterator it8 = index.begin();
			for (; it8 != index.end() ; it8++)
				std::cout << *it8 << " ";
			std::cout << std::endl;
			std::map<int, std::string> retur = locations[i].getReturn();
			std::map<int ,std::string>::iterator it9 = retur.begin();
			for (; it9 != retur.end() ; it9++)
				std::cout << it9->first << " " << it9->second << " ";
			std::cout << std::endl;
			std::cout << "----------------------------------------" << std::endl;
		}
	}
	return 0;
}
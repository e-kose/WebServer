/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HelperClass.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/04 18:10:20 by menasy            #+#    #+#             */
/*   Updated: 2025/06/29 11:34:04 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/poll.h>
#include "../include/LocationConf.hpp"
#include "../include/ServerConf.hpp"
#include "../include/WebServer.hpp"
#include <dirent.h>
#include <sys/stat.h>

class ServerConf;
class LocationConf;
class HelperClass;
class WebServer;
class HttpRequest;
class HelperClass
{
	private:
		HelperClass();
		HelperClass(const HelperClass &other);
		HelperClass &operator=(const HelperClass &other);
		~HelperClass();
		static std::vector<std::string> scriptExtVec;
		static std::vector<std::string> fileExtVec;
	
	public:
		static std::map<std::string, std::string>		findLocationCgi(LocationConf* locConf);
		static std::vector<std::string>					splitString(const std::string& str, char c);
		static std::vector<std::string> 				getScriptExtVec();
		static std::vector<std::string> 				getFileExtVec();
		static std::vector<std::string> 				selectLocOrServerIndex(const LocationConf* locConf, const std::vector<std::string>& serverIndexVec);
		static std::vector<std::string> 				selectTryFiles(const LocationConf* locConf, std::vector<LocationConf>& locVec);
		
		static void  									fillScriptExtVec();
		static void  									fillFileExtVec();
		static void 									printVector(const std::vector<std::string>& vec);
		static void										writeToFile(std::string, std::string);
		static void 									freeEnv(std::vector<char*>& env);
			
		static std::string 								intToString(int value);
		static std::string								trimLine(const std::string& str);
		static std::string								trimWithCharacter(const std::string& str, std::string characters);
		static std::string 								checkFileWithExtension(const std::string& path);
		static std::string 								fdToString(int& fd);
		static std::string 								getLocInVec(const std::string& path, const std::vector<LocationConf>& locVec);
		static std::string								indexHandler(const std::string& fullPath,const std::vector<std::string>& indexVec);
		static std::string								indexIsExist(ServerConf& conf, std::string location);
		static std::string								generateAutoIndexHtml(const std::string& path, const std::string& uriPath);
		static std::string								selectLocOrServerRoot(const LocationConf* locConf, const std::string& serverRoot);
		static std::string								checkEmptyAndTrim(std::string& value, std::string errorMsg);
		static std::string								createAndMove(std::string& str, std::string character);
		static std::string								mergeDirectory(const std::string& rootPath, const std::string& httpPath);
		static size_t 									characterCounter(const std::string& str, char c);
		static LocationConf* 							findLoc(const std::string& locPath, std::vector<LocationConf>& locVec);
		
		static bool										unchunkBody(const std::string& chunked, std::string& out);
		static bool										semiColonCheck(const std::string& str);
		static bool										isJustCharacter(const std::string& str, char c);
		static bool										fileIsExist(const std::string& path);
		static bool										strIsDigit(const std::string&);
		static bool 									isDirectory(const std::string& path);
		static bool 									isItScript(std::string extension);
		static bool 									requestSize(const ServerConf& conf, size_t contentLength);
		static bool										resolvePath(const std::string& path, std::string& out);
	};
	

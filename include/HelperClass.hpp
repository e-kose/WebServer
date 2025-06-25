/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HelperClass.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/04 18:10:20 by menasy            #+#    #+#             */
/*   Updated: 2025/06/25 12:36:56 by ekose            ###   ########.fr       */
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
#include "ServerConf.hpp"
#include <dirent.h>
#include <sys/stat.h>

class ServerConf;
class LocationConf;
class HelperClass
{
	private:
		HelperClass();
		HelperClass(const HelperClass &other);
		HelperClass &operator=(const HelperClass &other);
		~HelperClass();
	public:
		static std::string 					intToString(int value);
		static std::string					trimLine(const std::string& str);
		static std::string					trimWithCharacter(const std::string& str, std::string characters);
		static bool							semiColonCheck(const std::string& str);
		static bool							isJustCharacter(const std::string& str, char c);
		static size_t 						characterCounter(const std::string& str, char c);
		static std::vector<std::string>		splitString(const std::string& str, char c);
		static std::string					checkEmptyAndTrim(std::string& value, std::string errorMsg);
		static void 						printVector(const std::vector<std::string>& vec);
		static void							writeToFile(std::string, std::string);
		static std::string					createAndMove(std::string& str, std::string character);
		static std::string					mergeDirectory(const std::string& rootPath, const std::string& httpPath);
		static bool							fileIsExist(const std::string& path);
		static bool							strIsDigit(const std::string&);
		static	std::string 				checkFileWithExtension(const std::string& path, const std::map<std::string, std::string>& cgiExtMap);
		static int 							fileIsExecutable(const std::string& path, const std::string& extension, const std::map<std::string, std::string>& cgiExtMap);
		static std::string 					fdToString(int& fd);
		static std::string 					getLocInVec(const std::string& path, const std::vector<LocationConf>& locVec);
		// static std::string mergePath(const ServerConf& servConf, const LocationConf& locConf,const std::string reqFile, std::string& httpPath);
		static std::string					indexHandler(const std::string& fullPath,const std::vector<std::string>& indexVec);
			static std::string				indexIsExist(ServerConf& conf, std::string location);
		static std::string					generateAutoIndexHtml(const std::string& path, const std::string& uriPath);
		static bool 						isDirectory(const std::string& path);
		static LocationConf* 				findLoc(const std::string& locPath, std::vector<LocationConf>& locVec);
		static std::vector<std::string> 	selectLocOrServerIndex(const LocationConf* locConf, const std::vector<std::string>& serverIndexVec);
		static std::string					selectLocOrServerRoot(const LocationConf* locConf, const std::string& serverRoot);
		static bool							unchunkBody(const std::string& chunked, std::string& out);
		static std::map<std::string, std::string>	findLocationCgi(const std::vector<LocationConf> locVec, std::string locStr);
		static std::vector<std::string> 	selectTryFiles(const LocationConf* locConf, std::vector<LocationConf>& locVec);
	};

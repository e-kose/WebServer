/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HelperClass.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/04 18:10:20 by menasy            #+#    #+#             */
/*   Updated: 2025/05/15 15:50:23 by menasy           ###   ########.fr       */
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


class ServerConf;
class HelperClass
{
	private:
		HelperClass();
		HelperClass(const HelperClass &other);
		HelperClass &operator=(const HelperClass &other);
		~HelperClass();
		// static const std::string FILE_EXTENSIONS[6];
	public:
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
		// static std::string				readHtmlFile(const std::string& path);
		static std::string					mergeDirectory(const std::string& rootPath, const std::string& httpPath);
		// static std::string				createErrorResponse(const std::string& status, const ServerConf& conf, const std::string& rootPAth);
		static bool							fileIsExist(const std::string& path);
		static bool							strIsDigit(const std::string&);
		static std::string 					checkFileWithExtension(const std::string& path, const std::string& cgiExt);
		static int 							fileIsExecutable(const std::string& path, const std::string& extension, const std::string& cgiExtStr);

};

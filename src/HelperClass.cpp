/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HelperClass.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 07:50:19 by ekose             #+#    #+#             */
/*   Updated: 2025/05/27 20:42:48 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HelperClass.hpp"


HelperClass::HelperClass() {
}
HelperClass::HelperClass(const HelperClass &other) {
	*this = other;
}
HelperClass &HelperClass::operator=(const HelperClass &other) {
	if (this == &other) {
		return (*this);
	}
	return *this;
}
HelperClass::~HelperClass() {
}


size_t HelperClass::characterCounter(const std::string& str, char c)
{
	size_t counter = 0;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == c)
			counter++;
	}
	return counter;
}
bool HelperClass::isJustCharacter(const std::string& str, char c)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] != c)
		{
			if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
				continue;
			else
				return false;
		}
	}
	return true;
}
bool HelperClass::strIsDigit(const std::string& str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (!isdigit(str[i]))
			return false;
	}
	return true;
}

bool	HelperClass::semiColonCheck(const std::string& str)
{
	if (str.find_first_of(";") == std::string::npos)
	{
		if (isJustCharacter(str, '{') || isJustCharacter(str, '}') || str.find("location") != std::string::npos)
			return true;
		else
			return false;
	}
	if (str.find_first_of(";") != std::string::npos)
	{
		if (isJustCharacter(str,';') || str.find_first_of(";") != str.length() -1)
			return false;
		else
			return true;
	}
	return true;
}

std::string HelperClass::trimLine(const std::string& str) 
{
	
	std::string::size_type start = str.find_first_not_of(" \t\n\r");
	if (start == std::string::npos)
		return "";
	std::string::size_type end = str.find_last_not_of(" \t\n\r");
	return str.substr(start, end - start + 1);
}

std::string HelperClass::trimWithCharacter(const std::string& str, std::string characters)
{
	std::string::size_type start = str.find_first_not_of(characters);
	if (start == std::string::npos)
		return "";
	std::string::size_type end = str.find_last_not_of(characters);
	return str.substr(start, end - start + 1);
}

std::vector<std::string> HelperClass::splitString(const std::string& str, char c)
{
	std::vector<std::string> result;
	std::string::size_type start = 0;
	std::string::size_type end = str.find(c);

	while (end != std::string::npos) {
		result.push_back(trimLine(str.substr(start, end - start)));
		start = end + 1;
		end = str.find(c, start);
	}
	result.push_back(str.substr(start));

	return result;
}
std::string HelperClass::checkEmptyAndTrim(std::string& value, std::string errCode)
{
	std::string trimmedValue = trimLine(value);
	if (trimmedValue.empty() || isJustCharacter(trimmedValue, ';'))
		throw std::runtime_error(errCode + ": Value can not be empty");
	return trimmedValue;
}

void HelperClass::printVector(const std::vector<std::string>& vec)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		std::cout << "vec[" << i << "] = " << vec[i] << std::endl;
	}
}

void	HelperClass::writeToFile(std::string fileName, std::string message)
{
	std::cout << "*******>" << message << std::endl;
	std::ofstream ofs;
	ofs.open(fileName.c_str(), std::ofstream::out | std::ios_base::app);
	time_t now = time(NULL);
    std::string timeStr = ctime(&now);
    timeStr.erase(timeStr.length() - 1);  
	if (ofs.is_open())
	{
		ofs << "["<< timeStr << "] " << message << std::endl;
		ofs.close();
	}
	else
		std::cerr << "["<< timeStr << "] "<< "Error opening file: " << fileName << std::endl;
}


std::string HelperClass::createAndMove(std::string& str, std::string character)
{
	std::string dest;
	std::string::size_type pos;
	pos = str.find_first_of(character.c_str());
	dest = str.substr(0, pos);
	str = str.substr(pos + 1,str.length());
	return dest;
}

std::string HelperClass::checkFileWithExtension(const std::string& path, const std::map<std::string, std::string>& cgiExtMap)
{
	if (path.find_last_of(".") != std::string::npos)
		return path;
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		file.close();
		return path;
	}
	std::vector<std::string> extVec;
	extVec.push_back(".html");
	extVec.push_back(".txt");
	if (cgiExtMap.size() > 0)
		for (std::map<std::string, std::string>::const_iterator it = cgiExtMap.begin(); it != cgiExtMap.end(); it++)
			extVec.push_back(it->first);

	for (size_t i = 0; i < extVec.size(); i++)
	{
		std::string tmpPath = path + extVec[i];
		file.open(tmpPath.c_str());
		if (file.is_open())
			return tmpPath;
	}
	return path;
}
int 	HelperClass::fileIsExecutable(const std::string& path, const std::string& extension, const std::map<std::string, std::string>& cgiExtMap)
{
	
	for (std::map<std::string, std::string>::const_iterator it = cgiExtMap.begin(); it != cgiExtMap.end(); it++)
	{
		if (extension == it->first)
		{
			if (path.find("cgi-bin/") == std::string::npos)
				return -1;
			return 1;
		}
	}
	return 0;
}

std::string HelperClass::mergeDirectory(const std::string& rootPath, const std::string& httpPath)
{
	return	(rootPath + httpPath);
}

bool HelperClass::fileIsExist(const std::string& path)
{
	std::cout << "Checking if file exists: " << path << std::endl;
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		file.close();
		return true;
	}
	else
		return false;
}

std::string HelperClass::intToString(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
std::string HelperClass::fdToString(int& fd)
{
	char buffer[1024];
    std::string result;
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        result.append(buffer, bytesRead);
    }
	if (bytesRead == -1) {
		return "";
	}
	return result;
}

std::string HelperClass::indexIsExist(ServerConf& conf, std::string location)
{
	std::string  root = conf.getRoot();
	std::vector<std::string> indexVec;
	std::vector<LocationConf> locVec = conf.getLocations();
	std::vector<LocationConf>::iterator it = locVec.begin();
	for (; it != locVec.end(); it++)
	{
		if (it->getPath() == location)
		{
			if (it->getIndex().size() > 0)
			{
				indexVec = it->getIndex();
				if (!it->getRoot().empty())
					root = it->getRoot();
			}
			else
				indexVec = conf.getIndex();
			break;
		}
	}
	for (size_t i = 0; i < indexVec.size(); i++)
		if (HelperClass::fileIsExist(HelperClass::mergeDirectory(root + "/", indexVec[i])))
			return HelperClass::mergeDirectory(root + "/", indexVec[i]);
	return "errorPage";
}

std::string HelperClass::generateAutoIndexHtml(const std::string& path, const std::string& uriPath)
{
	DIR* dir = opendir(path.c_str());
	if (!dir) return "<html><body><h1>403 Forbidden</h1></body></html>";
	
	std::stringstream ss;
    ss << "<html><head><title>Index of " << uriPath << "</title></head><body>";
    ss << "<h1>Index of " << uriPath << "</h1><ul>";
    struct dirent* entry;
	
    while ((entry = readdir(dir)) != NULL) {
        if (std::string(entry->d_name) == ".") continue;
		
		std::string fullPath = path + "/" + entry->d_name;
		struct stat st;
		if (stat(fullPath.c_str(), &st) == 0)
		{
			if (S_ISDIR(st.st_mode))
                ss << "<li><a href=\"" << entry->d_name << "/\">" << entry->d_name << "/</a></li>";
            else
                ss << "<li><a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></li>";
        }
		else
			ss << "<li>" << entry->d_name << " (not accessible)</li>";
    }
    closedir(dir);
    ss << "</ul></body></html>";
    return ss.str();
}
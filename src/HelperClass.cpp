/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HelperClass.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 07:50:19 by ekose             #+#    #+#             */
/*   Updated: 2025/06/28 13:10:50 by menasy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HelperClass.hpp"
#include "../include/LocationConf.hpp"
#include "../include/ServerConf.hpp"
std::vector<std::string> HelperClass::scriptExtVec;
std::vector<std::string> HelperClass::fileExtVec;

HelperClass::HelperClass() 
{

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

void HelperClass::fillScriptExtVec() 
{
	scriptExtVec.push_back(".php");
	scriptExtVec.push_back(".py");
	scriptExtVec.push_back(".pl");
	scriptExtVec.push_back(".rb");
	scriptExtVec.push_back(".js");
}
void HelperClass::fillFileExtVec() 
{
	fileExtVec.push_back(".html");
	fileExtVec.push_back(".htm");
	fileExtVec.push_back(".css");
	fileExtVec.push_back(".txt");
	fileExtVec.push_back(".jpg");
	fileExtVec.push_back(".jpeg");
	fileExtVec.push_back(".png");
	fileExtVec.push_back(".gif");
	fileExtVec.push_back(".svg");
	fileExtVec.push_back(".ico");
}
std::vector<std::string> HelperClass::getScriptExtVec() {
    return scriptExtVec;
}
std::vector<std::string> HelperClass::getFileExtVec() {
    return fileExtVec;
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

std::string HelperClass::checkFileWithExtension(const std::string& path)
{
	if (path.find_last_of(".") != std::string::npos || HelperClass::fileIsExist(path))
		return path;
	std::ifstream file;
	std::vector<std::string> extVec;

	for (size_t i = 0; i < HelperClass::getFileExtVec().size(); i++)
		extVec.push_back(HelperClass::getFileExtVec()[i]);
	for (size_t i = 0; i < HelperClass::getScriptExtVec().size(); i++)
			extVec.push_back(HelperClass::getScriptExtVec()[i]);
	
	for (size_t i = 0; i < extVec.size(); i++)
	{
		std::string tmpPath = path + extVec[i];
		file.open(tmpPath.c_str());
		if (file.is_open())
			return tmpPath;
	}
	return path;
}


std::string HelperClass::indexHandler(const std::string& fullPath, const std::vector<std::string>& indexVec )
{
	std::string dest;
	std::string indexVal;
	dest = fullPath;
	for (size_t j = 0; j < indexVec.size(); j++)
	{
		indexVal = indexVec[j];
		dest += indexVal;
		if (HelperClass::fileIsExist(dest) && !HelperClass::isDirectory(dest))
			return indexVal;
	}
	return "";
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
		std::cout << "File exists true: " << path << std::endl;
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
std::string HelperClass::getLocInVec(const std::string& path, const std::vector<LocationConf>& locVec)
{
	std::string sub,resPath;
	size_t j = 0;
	while (path[j])
	{
		while (path[j] && path[j] != '/')
			j++;
		if (path[j] == '/')
			j++;
		sub = path.substr(0, j);
		for (size_t i = 0; i < locVec.size(); i++)
		{
			if (locVec[i].getPath() == sub)
				resPath = sub;
		}
	}
	return resPath;
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
		if (HelperClass::fileIsExist(HelperClass::mergeDirectory(root, indexVec[i])))
			return HelperClass::mergeDirectory(root, indexVec[i]);
	return "errorPage";
}

std::string HelperClass::generateAutoIndexHtml(const std::string& path, const std::string& uriPath)
{
	DIR* dir = opendir(path.c_str());
	if (!dir) {
		std::string body = "<html><body><h1>403 Forbidden</h1></body></html>";
		std::stringstream header;
		header << "HTTP/1.1 403 Forbidden\r\n"
		       << "Content-Type: text/html\r\n"
		       << "Content-Length: " << body.size() << "\r\n"
		       << "Connection: close\r\n\r\n"
		       << body;
		return header.str();
	}
	
	std::stringstream body;
	body << "<html><head><title>Index of " << uriPath << "</title></head><body>";
	body << "<h1>Index of " << uriPath << "</h1><ul>";
	
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") continue;

		std::string fullPath = path + "/" + entry->d_name;
		struct stat st;
		if (stat(fullPath.c_str(), &st) == 0) {
			if (S_ISDIR(st.st_mode))
				body << "<li><a href=\"" << entry->d_name << "/\">" << entry->d_name << "/</a></li>";
			else
				body << "<li><a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></li>";
		} else {
			body << "<li>" << entry->d_name << " (not accessible)</li>";
		}
	}
	closedir(dir);
	body << "</ul></body></html>";

	std::string html = body.str();
	std::stringstream header;
	header << "HTTP/1.1 200 OK\r\n"
	       << "Content-Type: text/html\r\n"
	       << "Content-Length: " << html.size() << "\r\n"
	       << "Connection: keep-alive\r\n\r\n"
	       << html;

	return header.str();
}
bool HelperClass::isDirectory(const std::string& path) 
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false; 
    return S_ISDIR(statbuf.st_mode);
}

LocationConf* HelperClass::findLoc(const std::string& locPath, std::vector<LocationConf>& locVec)
{
    for (size_t i = 0; i < locVec.size(); i++)
    {
        if (locVec[i].getPath() == locPath)
            return &locVec[i];
    }
    return NULL;
}
bool HelperClass::isItScript(std::string extension)
{
	std::vector<std::string> scriptVec = HelperClass::getScriptExtVec();
	std::cout << "Extension ısIt:" << extension << std::endl;
	for(size_t i = 0; i < scriptVec.size(); i++)
	{
		if(extension == scriptVec[i])
			return true;
	}
	return false; 	
}
std::map<std::string, std::string>HelperClass::findLocationCgi(LocationConf* locConf)
{
	std::map<std::string, std::string> cgiExtMap;
	if (locConf != NULL)
		cgiExtMap = locConf->getCgiExt();
	return cgiExtMap;
}
std::vector<std::string> HelperClass::selectLocOrServerIndex(const LocationConf* locConf, const std::vector<std::string>& serverIndexVec)
{
	std::vector<std::string> resVec;
	if (locConf == NULL)
		resVec = serverIndexVec;
	else
	{
		std::vector<std::string> locVec = locConf->getIndex();
		if (!locVec.empty())
			resVec = locVec;
		else
			resVec = serverIndexVec;
	}
	return resVec;
}

std::string HelperClass::selectLocOrServerRoot(const LocationConf* locConf, const std::string& serverRoot)
{
	std::string resRoot;
	if (locConf == NULL)
		resRoot = serverRoot;
	else
	{
		std::string locRoot = locConf->getRoot();
		if (!locRoot.empty())
			resRoot = locRoot;
		else
			resRoot = serverRoot;
	}
	return resRoot;
}
std::vector<std::string> HelperClass::selectTryFiles(const LocationConf* locConf, std::vector<LocationConf>& locVec)
{
	std::vector<std::string> resVec;
	LocationConf * rootLoc = HelperClass::findLoc("/", locVec);
	if (locConf == NULL)
	{
		if (rootLoc == NULL)
			return resVec;
		else
			return rootLoc->getTryFiles();
	}
	else
	{
		if (locConf->getTryFiles().size() == 0)
			return rootLoc->getTryFiles();
	}	
	return locConf->getTryFiles();
	
}

bool HelperClass::unchunkBody(const std::string& chunked, std::string& out)
{
    size_t pos = 0;
    while (true)
    {
        size_t newline = chunked.find("\r\n", pos);
        if (newline == std::string::npos)
            return false;

        std::string sizeStr = chunked.substr(pos, newline - pos);
        char* endptr = NULL;
        long chunkSize = std::strtol(sizeStr.c_str(), &endptr, 16);

        if (endptr == sizeStr.c_str() || *endptr != '\0' || chunkSize < 0)
            return false;

        pos = newline + 2;

        if (chunkSize == 0) {
            if (chunked.substr(pos, 2) != "\r\n")
                return false;
            return true;
        }

        if (chunked.size() < pos + chunkSize + 2)
            return false;

        out.append(chunked, pos, chunkSize);
        pos += chunkSize;

        if (chunked.substr(pos, 2) != "\r\n")
            return false;

        pos += 2;
    }
}

void HelperClass::freeEnv(std::vector<char*>& env) 
{
	for (std::vector<char*>::iterator it = env.begin(); it != env.end(); ++it)
		if (*it) delete[] *it;
	env.clear();
}
bool HelperClass::resolvePath(const std::string& path, std::string& out) {
	char resolved[PATH_MAX];
	if (realpath(path.c_str(), resolved) == NULL)
		return false;
	out = resolved;
	return true;
}


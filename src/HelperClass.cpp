/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HelperClass.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: menasy <menasy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 07:50:19 by ekose             #+#    #+#             */
/*   Updated: 2025/05/15 21:13:42 by menasy           ###   ########.fr       */
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
	
	std::string::size_type start = str.find_first_not_of(" \t\n");
	if (start == std::string::npos)
		return "";
	std::string::size_type end = str.find_last_not_of(" \t\n");
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
    std::string timeStr = ctime(&now); // Zamanı string'e çevir
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
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		file.close();
		return true;
	}
	else
		return false;
}


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

std::string HelperClass::trimLine(const std::string& str) {
	std::string::size_type start = str.find_first_not_of(" \t\n");
	if (start == std::string::npos)
		return "";
	std::string::size_type end = str.find_last_not_of(" \t\n");
	return str.substr(start, end - start + 1);
}
#include "../include/Tokenizer.hpp"

Tokenizer::Tokenizer(): Parser(){}

Tokenizer::Tokenizer(const Tokenizer& copy): Parser(copy) {
	*this = copy;
}

Tokenizer& Tokenizer::operator=(const Tokenizer& src) {
	if (this == &src) {
	   return (*this);
	}
	Parser::operator=(src);
	this->fullText = src.getFullText();
	return *this;
}

Tokenizer::~Tokenizer(){}

std::string Tokenizer::getFullText()const{
	return this->fullText;
}

void Tokenizer::setFullText(std::string fullText){
	this->fullText = fullText;
}

std::vector<std::string> Tokenizer::seperation(){
	std::string fullText;
	std::vector<std::string> text;
	std::string word;
	
	fullText = this->fullText;
	for (size_t i = 0; i < this->fullText.length(); i++){
		if (fullText[i] == '{' || fullText[i] == '}' || fullText[i] == ';' || fullText[i] == ' '){
			if (!word.empty()){
				text.push_back(word);
				word.clear();
			}
			if (fullText[i] == '{' || fullText[i] == '}' || fullText[i] == ';')
			{
				text.push_back(std::string(1, fullText[i]));
				text.push_back("\n");
			}
		}
		else if (fullText[i] != '\n')
			word.push_back(fullText[i]);
	}
	if (!word.empty())
		text.push_back(word);
	return text;
}

int Tokenizer::wordCounter(std::vector<std::string> vec, std::string searchWord){
	int	counter;
	std::vector<std::string>::iterator it;
	counter = 0;
	for (it = vec.begin(); it != vec.end(); it++){
		if(*it == searchWord || *it == ""+searchWord)
			counter++;
	}
	return counter;
}

bool stringToBool(const std::string& str) {
	
	std::string tmpValue = HelperClass::trimLine(str);
    return (tmpValue == "on" || tmpValue == "1");
}

LocationConf Tokenizer::createLocConf(std::vector<std::string>::iterator& it, std::vector<std::string> & sepVec){
	LocationConf locConf;
	std::map<std::string, int> confKeyCounter = this->confKeyMap("location");
	std::vector<std::string>::iterator end = sepVec.end();

	for ( ; it != end; it++){
		if ((it + 1) != end && *(it + 1) == "{") locConf.setPath(*it);
		else if (*it == "}") break;
		else if (*it == "root" && it++ != end && ++confKeyCounter["root"]) locConf.setRoot(*it);
		else if (*it == "upload_dir" && it++ != end && ++confKeyCounter["upload_dir"]) locConf.setUploadStore(*it);
		else if (*it == "autoindex" && it++ != end && ++confKeyCounter["autoindex"]) locConf.setAutoIndex(stringToBool(*it));
		else if (*it =="return" && it++ != end && ++confKeyCounter["return"])
		{
			int code = std::atoi(it->c_str());
			locConf.addReturn(code,*(++it));
		} 
		else if (*it == "cgi_ext" && it++ != end)
		{
			std::string tmpKey = *it;
			locConf.addCgiExt(tmpKey, *(++it));
		} 
		else if (*it == "try_files" && it++ != end && ++confKeyCounter["try_files"])
			for( ; *it != "\n" ; it++)
				locConf.addTryFiles(*it);
		else if (*it == "index" && it++ != end && ++confKeyCounter["index"])
			for( ; *it != "\n" ; it++)
				locConf.addIndex(*it);
		else if (*it == "methods" && it++ != end && ++confKeyCounter["methods"])
			for( ; *it != "\n" ; it++)
				locConf.addMethod(*it);
	}
	checkKeyCount(confKeyCounter);
	return locConf;
}

std::vector<ServerConf> Tokenizer::createConfVec(std::vector<std::string>& sepVec){

	std::vector<std::string>::iterator it;
	std::vector<ServerConf> serverVec(wordCounter(sepVec, "server"));
	std::map<std::string, int> confKeyCounter;
	size_t i;
	
	i = 0;
	it = sepVec.begin()++;
	confKeyCounter = this->confKeyMap("Server");
	while (i < serverVec.size())
	{
		for (;it != sepVec.end(); it++){
			if (*it == "server" && it != sepVec.begin() && ++it != sepVec.end()){
				break;
			} 
			else if (*it == "listen" && ++it != sepVec.end() && ++confKeyCounter["listen"]){
				size_t pos = it->find(':');
				if (pos != std::string::npos){
					serverVec[i].setIp(it->substr(0,pos));
					serverVec[i].setPort(std::atoi(it->substr(pos + 1).c_str()));
				}
				else{
					serverVec[i].setIp(*it);
					serverVec[i].setPort(7979);
				}
			}
			else if (*it == "error_page" && ++it != sepVec.end()){
				std::vector<int>			errorCode;
				std::string					filePath;
				std::vector<int>::iterator	codeIt;
				for( ; *it != "\n" ; it++){
					if (std::atoi(it->c_str()) != 0) errorCode.push_back(std::atoi(it->c_str()));
					else 
						for(codeIt = errorCode.begin() ; codeIt != errorCode.end() ; codeIt++)
							serverVec[i].addErrorPage(*codeIt, *it);
				}
				if (errorCode.size() == 0)
					serverVec[i].addErrorPage(0, *(it));
			}
			else if (*it == "server_name" && ++it != sepVec.end() && ++confKeyCounter["server_name"])
				for(; *it != "\n" ; it++) serverVec[i].addServerName(*it);
			else if (*it == "index" && ++it != sepVec.end() && ++confKeyCounter["index"])
				for(; *it != "\n" ; it++) serverVec[i].addIndex(*it);
			else if (*it == "root" && ++it != sepVec.end() && ++confKeyCounter["root"])
				serverVec[i].setRoot(*it);
			else if (*it == "access_log" && ++it != sepVec.end() && ++confKeyCounter["access_log"])
				serverVec[i].setAccesLog(*it);
			else if (*it == "error_log" && ++it != sepVec.end() && ++confKeyCounter["error_log"])
				serverVec[i].setErrorLog(*it);
			else if (*it == "client_max_body_size" && ++it != sepVec.end() && ++confKeyCounter["client_max_body_size"])
				serverVec[i].setBodySize(std::atoi(it->c_str()));
			else if (*it == "location" && ++it != sepVec.end())
			{
				LocationConf tmpLoc = createLocConf(it, sepVec);
				serverVec[i].addLocation(tmpLoc);
			}
		}
		checkKeyCount(confKeyCounter);
		confKeyCounter = this->confKeyMap("Server");
		i++;
	}
	return serverVec;
}
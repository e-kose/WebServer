#include "../include/Tokenizer.hpp"

Tokenizer::Tokenizer(): fullText(""){}

Tokenizer::Tokenizer(std::string fullText){
	this->fullText = (fullText);
}

Tokenizer::Tokenizer(const Tokenizer& copy) {
	*this = copy;
}

Tokenizer& Tokenizer::operator=(const Tokenizer& src) {
	if (this == &src) {
	   return (*this);
	}
	this->fullText = src.getFullText();
	return *this;
}

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
				text.push_back(std::string(1, fullText[i]));
			}
		else
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
		if(*it == searchWord || *it == "\n"+searchWord)
			counter++;
	}
	return counter;
}

bool stringToBool(const std::string& str) {
    return (str == "on" || str == "1");
}

LocationConf Tokenizer::createLocConf(std::vector<std::string>::iterator& it, std::vector<std::string> & sepVec){
	LocationConf locConf;
	std::map<std::string, int> confKeyCounter = this->confKeyMap("location");
	std::vector<std::string>::iterator end = sepVec.end();

	for ( ; it != end; it++){
		if ((it + 1) != end && *(it + 1) == "{") locConf.setPath(*it);
		else if (*it == "}") break;
		else if (*it == "\nroot" && it++ != end && ++confKeyCounter["root"]) locConf.setRoot(*it);
		else if (*it == "\ncgi_pass" && it++ != end && ++confKeyCounter["cgi_pass"]) locConf.setCgiPass(*it);
		else if (*it == "\ncgi_ext" && it++ != end && ++confKeyCounter["cgi_ext"]) locConf.setCgiExtension(*it);
		else if (*it == "\nupload_dir" && it++ != end && ++confKeyCounter["upload_dir"]) locConf.setUploadStore(*it);
		else if (*it == "\nautoindex" && it++ != end && ++confKeyCounter["autoindex"]) locConf.setAutoIndex(stringToBool(*it));
		else if (*it =="\nreturn" && it++ != end && ++confKeyCounter["return"]) locConf.addReturn(std::atoi(it->c_str()), *(++it));
		else if (*it == "\ntry_files" && it++ != end && ++confKeyCounter["try_files"])
			for( ; *it != ";" ; it++)
				locConf.addTryFiles(*it);
		else if (*it == "\nindex" && it++ != end && ++confKeyCounter["index"])
			for( ; *it != ";" ; it++)
				locConf.addIndex(*it);
		else if (*it == "\nmethods" && it++ != end && ++confKeyCounter["methods"])
			for( ; *it != ";" ; it++)
				locConf.addMethod(*it);
		checkKeyCount(confKeyCounter);
	}
	return locConf;
}

std::vector<ServerConf> Tokenizer::createConfVec(std::vector<std::string>& sepVec){

	std::vector<std::string>::iterator it;
	std::vector<ServerConf> serverVec(wordCounter(sepVec, "server"));
	int i;
	
	i = 0;
	it = sepVec.begin();
	it++;
	std::map<std::string, int> confKeyCounter = this->confKeyMap("Server");
	while (i < serverVec.size())
	{
		int check = 0;
		for (;it != sepVec.end() && check == 0 ; it++){
			if (*it == "\nserver" && it != sepVec.begin() && ++it != sepVec.end()){
				check++;
				continue;
			} 
			else if (*it == "\nlisten" && ++it != sepVec.end() && ++confKeyCounter["listen"]){
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
			else if (*it == "\nerror_page" && ++it != sepVec.end()){
				std::vector<int>			errorCode;
				std::string					filePath;
				std::vector<int>::iterator	codeIt;
				for( ; *it != ";" ; it++){
					if (std::atoi(it->c_str()) != 0) errorCode.push_back(std::atoi(it->c_str()));
					else 
						for(codeIt = errorCode.begin() ; codeIt != errorCode.end() ; codeIt++)
							serverVec[i].addErrorPage(*codeIt, *it);
				}
			}
			else if (*it == "\nserver_name" && ++it != sepVec.end() && ++confKeyCounter["server_name"])
				for(; *it != ";" ; it++) serverVec[i].addServerName(*it);
			else if (*it == "\nindex" && ++it != sepVec.end() && ++confKeyCounter["index"])
				for(; *it != ";" ; it++) serverVec[i].addIndex(*it);
			else if (*it == "\nroot" && ++it != sepVec.end() && ++confKeyCounter["root"])
				serverVec[i].setRoot(*it);
			else if (*it == "\naccess_log" && ++it != sepVec.end() && ++confKeyCounter["access_log"])
				serverVec[i].setAccesLog(*it);
			else if (*it == "\nerror_log" && ++it != sepVec.end() && ++confKeyCounter["error_log"])
				serverVec[i].setErrorLog(*it);
			else if (*it == "\nclient_max_body_size" && ++it != sepVec.end() && ++confKeyCounter["client_max_body_size"])
				serverVec[i].setBodySize(std::atoi(it->c_str()));
			else if (*it == "\nlocation" && ++it != sepVec.end())
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
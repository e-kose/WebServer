#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "ServerConf.hpp"
#include <cstdlib>

class Tokenizer{
	private:
		std::string	fullText;
	public:
		//Construtor and Desctructor
		Tokenizer();
		Tokenizer(std::string fullText);
		Tokenizer(const Tokenizer& copy);
		Tokenizer& operator=(const Tokenizer& src);
		
		//Getter Setter
		std::string		getFullText()const;
		void			setFullText(std::string fullText);
		
		//Auxiliary functions
		static int					wordCounter(std::vector<std::string> vec, std::string searchWord);
		LocationConf				createLocConf(std::vector<std::string>::iterator& it, std::vector<std::string> & sepVec);
		std::vector<std::string>	seperation();
		std::vector<ServerConf>		createConfVec(std::vector<std::string>& sepVec);
};
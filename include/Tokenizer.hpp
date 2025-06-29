#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "ServerConf.hpp"
#include <cstdlib>
#include "Parser.hpp"
#include "HelperClass.hpp"


class Tokenizer : public Parser{
	protected:
		std::string	fullText;
	public:
		Tokenizer();
		Tokenizer(const Tokenizer& copy);
		Tokenizer& operator=(const Tokenizer& src);
		~Tokenizer();
		
		std::string					getFullText()const;
		void						setFullText(std::string fullText);
		
		static int					wordCounter(std::vector<std::string> vec, std::string searchWord);
		LocationConf				createLocConf(std::vector<std::string>::iterator& it, std::vector<std::string> & sepVec);
		std::vector<std::string>	seperation();
		std::vector<ServerConf>		createConfVec(std::vector<std::string>& sepVec);
};
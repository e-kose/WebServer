#pragma once

#include <string>
#include <vector>
#include <iostream>

class Tokenizer{
	private:
		std::string fullText;
		std::vector<std::string> sepText;
	public:
		Tokenizer();
		Tokenizer(std::string fullText);
		Tokenizer(const Tokenizer& copy);
		Tokenizer& operator=(const Tokenizer& src);
		std::string getFullText()const;
		void setFullText(std::string fullText);
		std::vector<std::string> getText()const;
		void seperation();
};
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
std::vector<std::string> Tokenizer::getText()const{
	return this->sepText;
}
void Tokenizer::seperation(){
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
	this->sepText = text;
}
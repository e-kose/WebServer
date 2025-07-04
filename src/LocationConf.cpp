/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 14:24:16 by ekose             #+#    #+#             */
/*   Updated: 2025/07/04 09:08:21 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/LocationConf.hpp"

LocationConf::LocationConf()
{
	this->path = "";
	this->autoIndex = false;
	this->upload_store = "";
	this->root = "";
	this->methods = std::vector<std::string>();
	this->return_ = std::map<int, std::string>();
	this->try_files = std::vector<std::string>();
	this->index = std::vector<std::string>();
	this->cgi_ext = std::map<std::string, std::string>();
}
LocationConf::LocationConf(const LocationConf& copy) {
	*this = copy;
}

LocationConf& LocationConf::operator=(const LocationConf& src) {
	if (this == &src) {
	   return (*this);
	}
	this->path = src.path;
	this->methods = src.methods;
	this->autoIndex = src.autoIndex;
	this->upload_store = src.upload_store;
	this->root = src.root;
	this->index = src.index;
	this->try_files = src.try_files;
	this->return_ = src.return_;
	this->cgi_ext = src.cgi_ext;
	return *this;
}
LocationConf::~LocationConf() {
}
std::string LocationConf::getPath() const {

	return this->path;
}
std::vector<std::string> LocationConf::getMethods() const {
	return this->methods;
}
bool LocationConf::getAutoIndex() const {
	return this->autoIndex;
}
std::string LocationConf::getUploadStore() const {
	return this->upload_store;
}
std::map<int, std::string> LocationConf::getReturn() const {
	return this->return_;
}
std::string LocationConf::getRoot() const {
	return this->root;
}
std::vector<std::string> LocationConf::getTryFiles()const{
	return this->try_files;
}
std::vector<std::string> LocationConf::getIndex()const{
	return this->index;
}
std::map<std::string, std::string> LocationConf::getCgiExt() const {
	return this->cgi_ext;
}
void LocationConf::setPath(std::string path) {
	std::string tmpValue = HelperClass::checkEmptyAndTrim(path, "Path");
	this->path = tmpValue;
}
void LocationConf::setMethods(std::vector<std::string> methods) {

	std::string tmpValue;
	for (size_t i = 0; i < methods.size(); i++) {
		tmpValue = HelperClass::checkEmptyAndTrim(methods[i], "Method");
		if (tmpValue == "GET" || tmpValue == "POST" || tmpValue == "DELETE") {
			this->methods.push_back(tmpValue);
		} 
		else
			throw std::runtime_error("Invalid method"); 
	}
}
void LocationConf::setAutoIndex(bool autoIndex) {
	this->autoIndex = autoIndex;
}
void LocationConf::setUploadStore(std::string upload_store) {
	std::string tmpValue = HelperClass::checkEmptyAndTrim(upload_store, "Upload store");
	this->upload_store = tmpValue;
}
void LocationConf::setRoot(std::string root) {
	std::string tmpValue = HelperClass::checkEmptyAndTrim(root, "Root");
	this->root = tmpValue;
}

void LocationConf::addMethod(std::string method){ 
	if (method == ";" && this->methods.size() != 0)
		return;
	std::string tmpValue = HelperClass::checkEmptyAndTrim(method, "Method");
	if (tmpValue == "GET" || tmpValue == "POST" || tmpValue == "DELETE") {
		this->methods.push_back(tmpValue);
	} 
	else
		throw std::runtime_error("Invalid method");
}
void LocationConf::addTryFiles(std::string file){ 
	if (file == ";" && this->try_files.size() != 0)
		return;
	std::string tmpValue = HelperClass::checkEmptyAndTrim(file, "Try files");
	if (tmpValue.find_last_of("=") != std::string::npos)
		tmpValue = HelperClass::trimLine(tmpValue.substr(tmpValue.find_last_of("=") + 1, tmpValue.length()));
	std::string::size_type pos = tmpValue.find("$uri");
	if ((pos == std::string::npos || pos != 0) && !HelperClass::strIsDigit(tmpValue))
		throw std::runtime_error("Invalid format int files");
	if(!HelperClass::strIsDigit(tmpValue) && (tmpValue.length() > 4 && tmpValue.substr(4, tmpValue.length()).find("$uri") != std::string::npos))
		throw std::runtime_error("Invalid format int files");
	this->try_files.push_back(tmpValue);
}
void LocationConf::addIndex(std::string file){ 

	if (file == ";" && this->index.size() != 0)
		return;
	std::string tmpValue = HelperClass::checkEmptyAndTrim(file, "Index");
	this->index.push_back(tmpValue);
}
void LocationConf::addReturn(int code, std::string link){

	if (code < 100 || code > 599)
		throw std::runtime_error("Invalid return code");
	std::string tmpValue = HelperClass::checkEmptyAndTrim(link, "Return");
	this->return_[code] = tmpValue;
}
void LocationConf::addCgiExt(std::string ext, std::string path){
	std::string tmpValue = HelperClass::checkEmptyAndTrim(ext, "Cgi extension");
	std::string tmpPath = HelperClass::checkEmptyAndTrim(path, "Cgi path");
	this->cgi_ext[tmpValue] = tmpPath;
}

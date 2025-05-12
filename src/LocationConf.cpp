/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekose <ekose@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 14:24:16 by ekose             #+#    #+#             */
/*   Updated: 2025/05/12 08:05:20 by ekose            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/LocationConf.hpp"

LocationConf::LocationConf()
{
	this->path = "";
	this->autoIndex = false;
	this->upload_store = "";
	this->root = "";
	this->cgi_extension = "";
	this->cgi_path = "";
	this->methods = std::vector<std::string>();
	this->cgi_pass = "";
	this->return_ = std::map<int, std::string>();
	this->try_files = std::vector<std::string>();
	this->index = std::vector<std::string>();
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
	this->cgi_extension = src.cgi_extension;
	this->cgi_path = src.cgi_path;
	this->cgi_pass = src.cgi_pass;
	this->index = src.index;
	this->try_files = src.try_files;
	this->return_ = src.return_;
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
std::string LocationConf::getCgiPass() const {
	return this->cgi_pass;
}
std::map<int, std::string> LocationConf::getReturn() const {
	return this->return_;
}
std::string LocationConf::getRoot() const {
	return this->root;
}
std::string LocationConf::getCgiExtension() const {
	return this->cgi_extension;
}
std::string LocationConf::getCgiPath() const {
	return this->cgi_path;
}
std::vector<std::string> LocationConf::getTryFiles()const{
	return this->try_files;
}
std::vector<std::string> LocationConf::getIndex()const{
	return this->index;
}
void LocationConf::setPath(std::string path) {
	std::string tmpValue = HelperClass::checkEmptyAndTrim(path, "Path");
	this->path = tmpValue;
}
void LocationConf::setMethods(std::vector<std::string> methods) {//Kullanılmıyor

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
void LocationConf::setAutoIndex(bool autoIndex) {// Bunun kontrolu alındığı yerde yapıldı
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
void LocationConf::setCgiExtension(std::string cgi_extension) {
	std::string tmpValue = HelperClass::checkEmptyAndTrim(cgi_extension, "Cgi extension");
	this->cgi_extension = tmpValue;
}
void LocationConf::setCgiPath(std::string cgi_path) {
	std::string tmpValue = HelperClass::checkEmptyAndTrim(cgi_path, "Cgi path");
	this->cgi_path = tmpValue;
}
void LocationConf::setCgiPass(std::string cgi_pass) {
	std::string tmpValue = HelperClass::checkEmptyAndTrim(cgi_pass, "Cgi pass");
	this->cgi_pass = tmpValue;
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
	if (tmpValue[0] == '=')
		tmpValue = tmpValue.substr(1);
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

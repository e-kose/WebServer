#include "../include/LocationConf.hpp"

LocationConf::LocationConf() : path(""), autoIndex(false), upload_store(""), root(""), cgi_extension(""), cgi_path(""), methods(){
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
std::string LocationConf::getRoot() const {
	return this->root;
}
std::string LocationConf::getCgiExtension() const {
	return this->cgi_extension;
}
std::string LocationConf::getCgiPath() const {
	return this->cgi_path;
}
void LocationConf::setPath(std::string path) {
	this->path = path;
}
void LocationConf::setMethods(std::vector<std::string> methods) {
	this->methods = methods;
}
void LocationConf::setAutoIndex(bool autoIndex) {
	this->autoIndex = autoIndex;
}
void LocationConf::setUploadStore(std::string upload_store) {
	this->upload_store = upload_store;
}
void LocationConf::setRoot(std::string root) {
	this->root = root;
}
void LocationConf::setCgiExtension(std::string cgi_extension) {
	this->cgi_extension = cgi_extension;
}
void LocationConf::setCgiPath(std::string cgi_path) {
	this->cgi_path = cgi_path;
}
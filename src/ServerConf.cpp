#include "../include/ServerConf.hpp"

ServerConf::ServerConf(): 
	ip(""),
	port(0),
	root(""),
	server_name(),
	index(""),
	body_size(0),
	error_pages(),
	locations()
{
}
ServerConf::ServerConf(const ServerConf &other) {
	*this = other;
}

ServerConf &ServerConf::operator=(const ServerConf &other) {
	if (this == &other) {
		return (*this);
	}
	this->ip = other.ip;
	this->port = other.port;
	this->root = other.root;
	this->server_name = other.server_name;
	this->index = other.index;
	this->body_size = other.body_size;
	this->error_pages = other.error_pages;
	this->locations = other.locations;
	return *this;
}
ServerConf::~ServerConf() {
}

std::string ServerConf::getIp() const {
	return this->ip;
}
int ServerConf::getPort() const {
	return this->port;
}
std::string ServerConf::getRoot() const {
	return this->root;
}
std::vector<std::string> ServerConf::getServerName() const {
	return this->server_name;
}
std::string ServerConf::getIndex() const {
	return this->index;
}
size_t ServerConf::getBodySize() const {
	return this->body_size;
}
std::map<int, std::string> ServerConf::getErrorPages() const {
	return this->error_pages;
}
std::vector<LocationConf> ServerConf::getLocations() const {
	return this->locations;
}
void ServerConf::setIp(std::string ip) {
	this->ip = ip;
}
void ServerConf::setPort(int port) {
	this->port = port;
}
void ServerConf::setRoot(std::string root) {
	this->root = root;
}
void ServerConf::setServerName(std::vector<std::string>& server_name) {
	this->server_name = server_name;
}
void ServerConf::setIndex(std::string index) {
	this->index = index;
}
void ServerConf::setBodySize(size_t body_size) {
	this->body_size = body_size;
}
void ServerConf::setErrorPages(std::map<int, std::string>& error_pages) {
	this->error_pages = error_pages;
}
void ServerConf::setLocations(std::vector<LocationConf>& locations) {
	this->locations = locations;
}

void ServerConf::addLocation(LocationConf& location) {
	this->locations.push_back(location);
}
void ServerConf::addErrorPage(int code, std::string page) {
	this->error_pages[code] = page;
}

#include "../include/ServerConf.hpp"

ServerConf::ServerConf()
{
	this->ip = "";
	this->port = 7927;
	this->root = "";
	this->access_log = "";
	this->error_log = "";
	this->body_size = 0;
	this->error_pages = std::map<int, std::string>();
	// this->locations = std::vector<LocationConf>();
	// this->server_name = std::vector<std::string>();
	// this->index = std::vector<std::string>();
	this->defaultPage();
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
	this->access_log = other.access_log;
	this->error_log = other.error_log;
	this->error_pages = other.error_pages;
	this->locations = other.locations;
	this->server_name = other.server_name;
	this->index = other.index;
	this->body_size = other.body_size;
	this->dflt_page = other.dflt_page;
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
std::vector<std::string> ServerConf::getIndex() const {
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
std::string ServerConf::getErrorLog()const{
	return this->error_log;
}
std::string ServerConf::getAccessLog()const{
	return this->access_log;
}

std::map<int, std::string> ServerConf::getDfltPage() const {
	return this->dflt_page;
}

void ServerConf::setIp(std::string ip) 
{
	std::string tmpValue = HelperClass::trimLine(ip);
	if (tmpValue == "localhost")
		tmpValue = "127.0.0.1";
	this->ip = tmpValue;
}
void ServerConf::setPort(int port) 
{
	if (port < 0 || port > 65535)
		throw std::runtime_error("Invalid port number");
	this->port = port;
}

void ServerConf::setRoot(std::string root) {
	std::string tmpValue = HelperClass::checkEmptyAndTrim(root, "Root");
	this->root = tmpValue;
}

void ServerConf::setBodySize(size_t body_size) {
	if (body_size <= 0)
		throw std::runtime_error("Invalid body size");
	this->body_size = body_size;
}

void	ServerConf::setLocations(std::vector<LocationConf>& locations)
{
	this->locations = locations;
}

void ServerConf::addLocation(LocationConf location) {
	this->locations.push_back(location);
}

void ServerConf::addErrorPage(int code, std::string page) {
	if (code < 100 || code > 599)
		throw std::runtime_error("Invalid error code");
	if (page == ";" && this->error_pages.size() != 0 && this->error_pages[code] != "")
		return;
	std::string tmpValue = HelperClass::checkEmptyAndTrim(page, "Error page");
	this->error_pages[code] = tmpValue;
}

void ServerConf::addServerName(std::string name){
	if (name == ";" && this->server_name.size() != 0)
		return;
	std::string tmpValue = HelperClass::checkEmptyAndTrim(name, "Server name");
	this->server_name.push_back(tmpValue);
}

void ServerConf::addIndex(std::string index){
	if (index == ";" && this->index.size() != 0)
		return;
	std::string tmpValue = HelperClass::checkEmptyAndTrim(index, "Index");
	this->index.push_back(tmpValue);
}

void ServerConf::setErrorLog(std::string logFile){
	std::string tmpValue = HelperClass::checkEmptyAndTrim(logFile, "Error log");
	this->error_log = tmpValue;
}

void ServerConf::setAccesLog(std::string logFile){
	std::string tmpValue = HelperClass::checkEmptyAndTrim(logFile, "Access log");
	this->access_log = tmpValue;
}

void ServerConf::defaultPage()
{
	this->dflt_page[200] = "<!DOCTYPE html>\n<html>\n<head><title>Webserv</title></head>\n<body><h1>Welcome to Webserv!</h1></body>\n</html>\n";
	this->dflt_page[400] = "<html><body><h1>400 Bad Request</h1><p>Sunucu isteğinizi anlayamadı.</p></body></html>";
	this->dflt_page[401] = "<html><body><h1>401 Unauthorized</h1><p>Kimlik doğrulama gerekiyor.</p></body></html>";
	this->dflt_page[403] = "<html><body><h1>403 Forbidden</h1><p>Erişim izniniz yok.</p></body></html>";
	this->dflt_page[404] = "<html><body><h1>404 Not Found</h1><p>Sayfa bulunamadı.</p></body></html>";
	this->dflt_page[405] = "<html><body><h1>405 Method Not Allowed</h1><p>HTTP metodu desteklenmiyor.</p></body></html>";
	this->dflt_page[413] = "<html><body><h1>413 Payload Too Large</h1><p>İstek çok büyük.</p></body></html>";
	this->dflt_page[500] = "<html><body><h1>500 Internal Server err</h1><p>Sunucu hatası oluştu.</p></body></html>";
	this->dflt_page[501] = "<html><body><h1>501 Not Implemented</h1><p>İşlev desteklenmiyor.</p></body></html>";
	this->dflt_page[503] = "<html><body><h1>503 Service Unavailable</h1><p>Sunucu hizmet veremiyor.</p></body></html>";
}
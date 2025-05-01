#include "../include/LocationConf.hpp"

LocationConf::LocationConf() : 
	path(""),
	autoIndex(false),
	upload_store(""),
	root(""),
	cgi_extension(""),
	cgi_path(""),
	methods(),
	cgi_pass(""),
	return_(),
	try_files(),
	index()
{
}
LocationConf::LocationConf(const LocationConf& copy) {
	*this = copy;
}

LocationConf& LocationConf::operator=(const LocationConf& src) {
	if (this == &src) {
	   return (*this);
	}
	this->path = src.getPath();
	this->methods = src.getMethods();
	this->autoIndex = src.getAutoIndex();
	this->upload_store = src.getUploadStore();
	this->root = src.getRoot();
	this->cgi_extension = src.getCgiExtension();
	this->cgi_path = src.getCgiPath();
	this->cgi_pass = src.getCgiPass();
	this->index = src.getIndex();
	this->try_files = src.getTryFiles();
	this->return_ = src.getReturn();
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
void LocationConf::setCgiPass(std::string cgi_pass) {
	this->cgi_pass = cgi_pass;
}
void LocationConf::setTryFiles(std::vector<std::string> try_files) {
	this->try_files = try_files;
}
void LocationConf::setReturn(std::map<int, std::string> link){
	this->return_ = link;
}
void LocationConf::setIndex(std::vector<std::string> index) {
	this->index = index;
}
void LocationConf::addMethod(std::string method){
	this->methods.push_back(method);
}
void LocationConf::addTryFiles(std::string file){
	this->try_files.push_back(file);
}
void LocationConf::addIndex(std::string file){
	this->index.push_back(file);
}
void LocationConf::addReturn(int code, std::string link){
	this->return_[code] = link;
}

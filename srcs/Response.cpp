#include "../Class/Response.hpp"
#include "../Class/ResponseHeader.hpp"

#include <dirent.h>


std::string			Response::getAutoIndexPage(const char *path, std::string const &host, int port) {
	std::string dirName(path);
	DIR *dir = opendir(path);
	std::string page =\
	"<!DOCTYPE html>\n\
	<html>\n\
	<head>\n\
			<title>" + dirName + "</title>\n\
	</head>\n\
	<body>\n\
	<h1>INDEX</h1>\n\
	<p>\n";

	if (dir == NULL) {
		std::cerr << "Error: could not open [" << path << "]" << std::endl;
		return "";
	}
	if (dirName[0] != '/')
		dirName = "/" + dirName;
	for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) {
		std::stringstream   ss;
		ss << "\t\t<p><a href=\"http://" + host + ":" << port <<\
			_uri + "/" + std::string(dirEntry->d_name) + "\">" + std::string(dirEntry->d_name) + "</a></p>\n";
		page += ss.str();
	}
	page +="\
	</p>\n\
	</body>\n\
	</html>\n";
	closedir(dir);
	return page;
}

int		pathIsFile(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0 ) {
		if (s.st_mode & S_IFREG)
			return 1;
	}
	return 0;
}


// Static Assets

std::map<std::string, void (Response::*)(Request &, RequestConfig &)>	Response::initMethods()
{
	std::map<std::string, void (Response::*)(Request &, RequestConfig &)> map;

	map["GET"] = &Response::getMethod;
	map["POST"] = &Response::postMethod;
	map["DELETE"] = &Response::deleteMethod;

	return map;
}

std::map<std::string, void (Response::*)(Request &, RequestConfig &)> Response::_method = Response::initMethods();

// Methods

void			Response::call(Request & request, RequestConfig & requestConf) {
	_code = request.getCode();
	// if ( _code != 200 ) {
	// 	_response = this->readHtml( _errors_map[ std::to_string(_code)] );
	// 	ResponseHeader	respHead( _code, _path.c_str(), _response.size(), ".html" );
	// 	_response = respHead.getResponseHeader() + _response + "\r\n";
	// 	return ;
	// }
	_uri = request.getRequestElem().find("path")->second;
	_path = requestConf.getPath();
std::cout << "uri: " << _uri << std::endl;
std::cout << "path: " << _path << std::endl;
	std::cout << "received path: " << _path << std::endl;
	_errors_map = requestConf.get_error_pages();
	_isAutoIndex = requestConf.get_autoindex();

	std::map<std::string, std::string> elems = request.getRequestElem();
	
	std::vector<std::string> allowed_methods = requestConf.get_methods();
	if ( _method.find(elems["method"]) == _method.end() ) {
		std::cerr << "\033[0;31mUnknown method: \'" << elems["method"] << "\'!!\n\033[0;0m";
		_code = 405;
	}
	else if ( std::find( allowed_methods.begin(), allowed_methods.end(), elems["method"] ) == allowed_methods.end() )
		_code = 405;
	else if (requestConf.get_client_max_body_size() < request.getRequest().size())
		_code = 413;
	if (_code == 405 || _code == 413)
	{
		_response = this->readHtml( _errors_map[ std::to_string(_code)] );
		ResponseHeader	respHead( _code, _path.c_str(), _response.size(), ".html" );
		_response = respHead.getResponseHeader() + _response + "\r\n";
		return ;
	}
	(this->*Response::_method[elems["method"]])(request, requestConf);
}

void			Response::getMethod(Request & request, RequestConfig & requestConfig ) {
	std::cout << "-- Call GET --\n";

	// if (cgipath!=null) {
	// 	do smthg
	// }
	// else
	if ( _code == 200 )
		_code = readContent();
	else // if 400 or cgi error
		_response = this->readHtml( _errors_map[ std::to_string(_code)] );
	if ( _code == 500 ) // if open file error
		_response = this->readHtml( _errors_map[ std::to_string(_code)] );


	// SET RESPONSE
	ResponseHeader	respHead( _code, _path.c_str(), _response.size(), ".html" );
	this->_response = respHead.getResponseHeader() + _response + "\r\n";
	std::cout << this->_response << std::endl; //debug
}

void			Response::postMethod(Request & request, RequestConfig & requestConfig) {
	std::cout << "-- Call POST --\n";
}

void			Response::deleteMethod(Request & request, RequestConfig & requestConfig) {
	std::cout << "-- Call DELETE --\n";
}

int				Response::readContent(void)
{
	std::ifstream		file;
	std::stringstream	buffer;

	_response = "";

	// std::cout << YELLOW << _path << RESET << std::endl;
	if (pathIsFile(_path))
	{
		file.open(_path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			_response = this->readHtml(_errors_map["403"]);
			return (403);
		}

		buffer << file.rdbuf();
		_response = buffer.str();

		file.close();
	}
	else if (_isAutoIndex) {
std::cout << "trying autoindex" << std::endl;
		buffer << getAutoIndexPage(_path.c_str(), "localhost", 8081);
		_response = buffer.str();
		// _type = "text/html";
	}
	else
	{
		_response = this->readHtml(_errors_map["404"]);
		return (404);
	}

	return (200);
}

std::string		Response::readHtml(const std::string& path)
{
	std::ofstream		file;
	std::stringstream	buffer;

std::cout << "trying to read: " << path << std::endl;
	if (pathIsFile(path))
	{
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");

		buffer << file.rdbuf();
		file.close();
		// _type = "text/html";

		return (buffer.str());
	}
	else
		return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");
}

std::string		Response::getResponse() { return _response; }

// -structors

Response::Response() {}
Response::~Response() {}

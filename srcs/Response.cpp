#include "../Class/Response.hpp"
#include "../Class/ResponseHeader.hpp"
#include "../Class/CGI.hpp"

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
	_uri = request.getRequestElem().find("path")->second;
	_host = requestConf.getHost();
	_port = requestConf.getPort();
	_path = requestConf.getPath();

	_errors_map = requestConf.get_error_pages();
	_isAutoIndex = requestConf.get_autoindex();
	if ( _path.find('.') != std::string::npos ) {
		_type = _path.substr( _path.find_last_of('.') );
	}

	std::map<std::string, std::string> elems = request.getRequestElem();
	
	std::vector<std::string> allowed_methods = requestConf.get_methods();
	if ( _method.find(elems["method"]) == _method.end() ) {
		std::cerr << "\033[0;31mUnknown method: \'" << elems["method"] << "\'!!\n\033[0;0m";
		_code = 405;
	}
	else if ( std::find( allowed_methods.begin(), allowed_methods.end(), elems["method"] ) == allowed_methods.end() )
		_code = 405;
	else if (requestConf.get_client_max_body_size() < (size_t)request.getBodySize())
		_code = 413;
	if (_code == 405 || _code == 413)
	{
		_response = this->readHtml( _errors_map[ std::to_string(_code)] );
		ResponseHeader	respHead( _code, _path.c_str(), _response.size(), _type.c_str() );
		_response = respHead.getResponseHeader() + _response + "\r\n";
		return ;
	}
	(this->*Response::_method[elems["method"]])(request, requestConf);
}

void			Response::getMethod(Request & request, RequestConfig & requestConfig ) {
	if (isCGI(requestConfig.get_cgi_ext()) && fileExists(requestConfig.getPath())) {
		CGI cgi(request, requestConfig);
		_response = cgi.getResponse();
		if (cgi.getStatus())
			_code = cgi.getStatus();
	}
	else if (_code == 200)
		_code = readContent();
	if (_code == 400)
		_response = this->readHtml( _errors_map[std::to_string(_code)]);
	if (_code == 500) // if open file error
		_response = this->readHtml( _errors_map[std::to_string(_code)]);
	// SET RESPONSE
	ResponseHeader	respHead(_code, _path.c_str(), _response.size(), _type.c_str());
	this->_response = respHead.getResponseHeader() + _response + "\r\n";
}

void			Response::postMethod(Request & request, RequestConfig & requestConfig) {
	if (isCGI(requestConfig.get_cgi_ext()) && fileExists(requestConfig.getPath())) {
		CGI	cgi( request, requestConfig);
		_response = cgi.getResponse();
		if (cgi.getStatus())
			_code = cgi.getStatus();
		if ( _code == 400 )
			_response = this->readHtml( _errors_map[ std::to_string(_code)] );
		else if ( _code == 500 ) // if open file error
			_response = this->readHtml( _errors_map[ std::to_string(_code)] );
	}
	else {
		_code = 204;
		for (size_t i = 0; i < request.getFilenames().size(); i++) {
			std::string filename = requestConfig.get_post_path() + request.getFilenames()[i].c_str();
			std::ofstream file(filename);
			if (file.is_open()) {
				file << request.getFiles()[request.getFilenames()[i]];
				file.close();
			}
			else
				; // error opening file
		}
	}
	// SET RESPONSE
	ResponseHeader	respHead(_code, _path.c_str(), _response.size(), _type.c_str());
	this->_response = respHead.getResponseHeader() + _response + "\r\n";
}

void			Response::deleteMethod(Request & request, RequestConfig & requestConfig) {
	(void)	request;
	(void)	requestConfig;

	_response = "";
	if (pathIsFile(_path))
	{
		if (remove(_path.c_str()) == 0)
			_code = 204;
		else
			_code = 403;
	}
	else
		_code = 404;
	if (_code == 403 || _code == 404)
		_response = this->readHtml( _errors_map[ std::to_string(_code)] );
	ResponseHeader	respHead( _code, _path.c_str(), _response.size(), _type.c_str() );
	this->_response = respHead.getResponseHeader() + _response + "\r\n";
}

int				Response::readContent(void)
{
	std::ifstream		file;
	std::stringstream	buffer;

	_response = "";

	if (pathIsFile(_path))
	{
		file.open(_path.c_str(), std::ios::binary);
		if (file.is_open() == false)
		{
			_response = this->readHtml(_errors_map["403"]);
			return (403);
		}

		buffer << file.rdbuf();
		// _response = buffer.str() + "\r\n";
		_response = buffer.str();

		file.close();
	}
	else if (_isAutoIndex) {
		buffer << getAutoIndexPage(_path.c_str(), _host, _port);
		_response = buffer.str();
		if ( _response == "" ) {
			_response = this->readHtml(_errors_map["404"]);
			return (404);
		}
		_type = "text/html";
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

	if (pathIsFile(path))
	{
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\r\n");

		buffer << file.rdbuf();
		file.close();
		_type = "text/html";

		return ( buffer.str() + "\r\n" );
	}
	else
		return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\r\n");
}

bool Response::fileExists(const std::string& filename) const {
    std::ifstream file(filename.c_str());
    return file.good();
}

bool	Response::isCGI(const std::vector<std::string>& extensions ) const {
	std::string::size_type pos = _path.find_last_of('.');
	std::string extension;

    if (pos != std::string::npos && pos != _path.length() - 1) {
        extension = _path.substr(pos + 1);
    } else {
		return false;
    }
	if (extensions.empty())
		return false;
	if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
        return true;
    }
	return false;
}

std::string		Response::getResponse() { return _response; }

// -structors

Response::Response() {}
Response::~Response() {}

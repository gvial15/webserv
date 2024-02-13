#include "../Class/Response.hpp"
#include "../Class/ResponseHeader.hpp"


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

void			Response::call(Request & request, RequestConfig & requestConfig) {
	std::map<std::string, std::string> elems = request.getRequestElem();
	if ( _method.find(elems["method"]) == _method.end() ) {
		std::cerr << "\033[0;31mUnknown method: \'" << elems["method"] << "\'!!\n\033[0;0m";
	}
	(this->*Response::_method[elems["method"]])(request, requestConfig);
}

void			Response::getMethod(Request & request, RequestConfig & requestConfig ) {
	std::cout << "-- Call GET --\n";

	std::string path = requestConfig.getPath();
	std::cout << "received path: " << path << std::endl;

	// READ REQUEST
	std::ifstream		file;
	std::stringstream	buffer;

	file.open(path.c_str(), std::ifstream::in);
	if ( file.is_open() ) {
		_code = 200;
		buffer << file.rdbuf();
		file.close();
	} else {
		std::cerr << "open() failed" << std::endl;
		_code = 404;
		// this->_response = errorMap[404];
		file.open(requestConfig.get_error_pages().find( "404" )->second, std::ifstream::in);
		std::cerr << "Trying to open file at: " << requestConfig.get_error_pages().find( std::to_string( _code ) )->second << std::endl;
		if ( file.is_open() ) {
			buffer << file.rdbuf();
			file.close();
		std::cerr << "buffer:" << buffer.str() << std::endl;
		} else {
			std::cerr << " Open Error File Failed !!!!!!" << std::endl;
			return;
		}
	}


	// SET RESPONSE
	_body = buffer.str();
	ResponseHeader	respHead( _code, path.c_str(), _body.size(), ".html" );
	this->_header.append( respHead.getResponseHeader() );
	this->_response = _header + _body;
	// std::cout << this->_response << std::endl; //debug
}

void			Response::postMethod(Request & request, RequestConfig & requestConfig) {
	std::cout << "-- Call POST --\n";
}

void			Response::deleteMethod(Request & request, RequestConfig & requestConfig) {
	std::cout << "-- Call DELETE --\n";
}

std::string		Response::getResponse() { return _response; }

// -structors

Response::Response() {}
Response::~Response() {}

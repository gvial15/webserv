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
	if (file.is_open() == false) {
		std::cerr << "open() failed";
		_code = 404;
		// this->_response = errorMap[404];
		return;
	}
	buffer << file.rdbuf();
	file.close();


	// SET RESPONSE
	_code = 200;
	this->_response = buffer.str();
	ResponseHeader	respHead(_code, path.c_str(), this->_response.size(), ".html");
	this->_response = respHead.getResponseHeader() + this->_response;
	// std::cout << this->_response << std::endl; //debug
}

void			Response::postMethod(Request & request, RequestConfig & requestConfig) {
	std::cout << "-- Call POST --\n";
}

void			Response::deleteMethod(Request & request, RequestConfig & requestConfig) {
	std::cout << "-- Call DELETE --\n";
}

std::string		Response::getResponse() { return this->_response; }

// -structors

Response::Response() {}
Response::~Response() {}

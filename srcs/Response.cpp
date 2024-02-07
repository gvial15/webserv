#include "../Class/Response.hpp"
#include "../Class/ResponseHeader.hpp"


// Static Assets

std::map<std::string, void (Response::*)(Request &, Server &)>	Response::initMethods()
{
	std::map<std::string, void (Response::*)(Request &, Server &)> map;

	map["GET"] = &Response::getMethod;
	map["POST"] = &Response::postMethod;
	map["DELETE"] = &Response::deleteMethod;

	return map;
}

std::map<std::string, void (Response::*)(Request &, Server &)> Response::_method = Response::initMethods();

// Methods

void			Response::call(Request & request, Server & server) {
	std::map<std::string, std::string> elems = request.getRequestElem();
	(this->*Response::_method[elems["method"]])(request, server);
}

void			Response::getMethod(Request & request, Server & server ) {
	std::cout << "-- Call GET --\n";

	// GET PATH DEPENDING ON LOCATION / ROOTS / INDEXES
	std::vector<std::string>::iterator							it;
	std::string path = const_cast<std::map<std::string, std::string>&>(request.getRequestElem())["path"];

	it = server.get_index().begin();
	while(it != server.get_index().end()) // check with index file only
	{
		it++;
	}
	if ( path == "/" )
		path = server.get_root() + server.get_index()[0];
	else
		path = server.get_root() + path;
	// std::cout << path << std::endl; //debug


	// READ REQUEST
	std::ifstream		file;
	std::stringstream	buffer;

	file.open(path.c_str(), std::ifstream::in);
	if (file.is_open() == false) {
		std::cerr << "open() failed";
		return;
	}
	buffer << file.rdbuf();
	file.close();


	// SET RESPONSE
	this->_response = buffer.str();
	ResponseHeader	respHead(200, path.c_str(), this->_response.size(), ".html");
	this->_response = respHead.getResponseHeader() + this->_response;
	// std::cout << this->_response << std::endl; //debug
}

void			Response::postMethod(Request & request, Server & server) {
	std::cout << "-- Call POST --\n";
}

void			Response::deleteMethod(Request & request, Server & server) {
	std::cout << "-- Call DELETE --\n";
}

std::string		Response::getResponse() { return this->_response; }

// -structors

Response::Response() {}
Response::~Response() {}

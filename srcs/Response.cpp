#include "../Class/Response.hpp"


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

void			Response::call(Request & request, Server & server)
{
	std::map<std::string, std::string> elems = request.getRequestElem();
	(this->*Response::_method[elems["method"]])(request, server);
}

void			Response::getMethod(Request & request, Server & server ) {
	std::cout << "-- Call GET --\n";

	std::ifstream		file;
	std::stringstream	buffer;

	std::string _response = "";

	std::map<std::string, std::string> elems = request.getRequestElem();
	std::cout << (server.get_root() + elems["path"]) << std::endl;
	file.open((server.get_root() + elems["path"]).c_str(), std::ifstream::in);
	if (file.is_open() == false) {
		std::cerr << "open() failed";
		return;
	}

	buffer << file.rdbuf();
	_response = buffer.str();
	std::cout << _response << std::endl;

	file.close();
}

void			Response::postMethod(Request & request, Server & server) {
	std::cout << "-- Call POST --\n";
}

void			Response::deleteMethod(Request & request, Server & server) {
	std::cout << "-- Call DELETE --\n";
}


// -tors

Response::Response() {}
Response::~Response() {}

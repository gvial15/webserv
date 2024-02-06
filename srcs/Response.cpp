#include "../Class/Response.hpp"


// Static Assets

std::map<std::string, void (Response::*)(Request &, Configuration &)>	Response::initMethods()
{
	std::map<std::string, void (Response::*)(Request &, Configuration &)> map;

	map["GET"] = &Response::getMethod;
	map["POST"] = &Response::postMethod;
	map["DELETE"] = &Response::deleteMethod;

	return map;
}

std::map<std::string, void (Response::*)(Request &, Configuration &)> Response::_method = Response::initMethods();

// Methods

void			Response::call(Request & request, Configuration & conf)
{
	std::string method = "GET";
	(this->*Response::_method[method])(request, conf);
}

void			Response::getMethod(Request & request, Configuration & conf) {
	std::cout << "-- Call GET --\n";
}

void			Response::postMethod(Request & request, Configuration & conf) {
	std::cout << "-- Call POST --\n";
}

void			Response::deleteMethod(Request & request, Configuration & conf) {
	std::cout << "-- Call DELETE --\n";
}


// -tors

Response::Response() {}
Response::~Response() {}

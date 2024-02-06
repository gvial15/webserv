#pragma once
#include <map>
// #include "../Class/Request.hpp"
#include "../Class/Configuration.hpp"

class Request;

class Response {
	public:
		Response();
		~Response();

		void			call(Request & request, Configuration & conf);

		void			getMethod(Request & request, Configuration & conf);
		void			postMethod(Request & request, Configuration & conf);
		void			deleteMethod(Request & request, Configuration & conf);

	private:

		static std::map<std::string, void (Response::*)(Request &, Configuration &)>	_method;
		static std::map<std::string, void (Response::*)(Request &, Configuration &)>	initMethods();
};
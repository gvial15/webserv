#pragma once
#include <map>
#include "../Class/Request.hpp"
#include "../Class/Configuration.hpp"
#include "../Class/Server.hpp"

class Response {
	public:
		Response();
		~Response();

		void			call( Request & request, Server & server );

		void			getMethod( Request & request, Server & server );
		void			postMethod( Request & request, Server & server );
		void			deleteMethod( Request & request, Server & server );

		std::string		getResponse();

	private:
		std::string	_response;

		static std::map<std::string, void (Response::*)(Request &, Server &)>	_method;
		static std::map<std::string, void (Response::*)(Request &, Server &)>	initMethods();
};
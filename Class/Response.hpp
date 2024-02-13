#pragma once
#include <map>
#include "../Class/Request.hpp"
#include "../Class/Configuration.hpp"
#include "../Class/RequestConfig.hpp"

class Response {
	public:
		Response();
		~Response();

		void			call( Request & request, RequestConfig & reqConfig );

		void			getMethod( Request & request, RequestConfig & reqConfig );
		void			postMethod( Request & request, RequestConfig & reqConfig );
		void			deleteMethod( Request & request, RequestConfig & reqConfig );

		std::string		getResponse();

	private:
		std::string	_body;
		std::string	_header;
		std::string	_response;
		int			_code;

		static std::map<std::string, void (Response::*)(Request &, RequestConfig &)>	_method;
		static std::map<std::string, void (Response::*)(Request &, RequestConfig &)>	initMethods();
};

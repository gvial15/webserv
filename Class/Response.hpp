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
		int				readContent(void);
		std::string		readHtml(const std::string& path);

		bool			isCGI( const std::vector<std::string>& extensions ) const;
		bool			fileExists(const std::string& filename) const;

	private:
		std::string							_path;
		std::string							_uri;
		std::string							_type;
		std::string							_response;
		int									_code;
		bool								_isAutoIndex;
		std::map<std::string, std::string>	_errors_map;

		std::string							_host;
		int									_port;

		static std::map<std::string, void (Response::*)(Request &, RequestConfig &)>	_method;
		static std::map<std::string, void (Response::*)(Request &, RequestConfig &)>	initMethods();
		std::string			getAutoIndexPage(const char *path, std::string const &host, int port);
};

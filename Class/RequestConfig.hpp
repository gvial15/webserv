#pragma once
#include <map>
#include "../Class/Request.hpp"
#include "../Class/Configuration.hpp"
#include "../Class/Server.hpp"
#include "SharedServerDirectives.hpp"

class RequestConfig: public SharedConfigDirectives {
	public:
		RequestConfig();
		RequestConfig( Request &, Server * );
		~RequestConfig();

		std::string	getPath() { return _path; }
		std::string	getHost() { return _host; }
		int			getPort() { return _port; }

	private:
		std::string											_path;
		std::map<std::string, Server::Location>::iterator	_location;
		std::map<std::string, Server::Location>				_locationsMap;

		std::string											_host;
		int													_port;


		void	findLocation();
		void	copyLocationConfig();
		void	pathRouting();
};
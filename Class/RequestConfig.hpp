#pragma once
#include <map>
#include "../Class/Request.hpp"
#include "../Class/Configuration.hpp"
#include "../Class/Server.hpp"
#include "../Class/SharedServerAttributes.hpp"

class RequestConfig: public SharedConfigAttributes {
	public:
		RequestConfig();
		RequestConfig( Request &, Server * );
		~RequestConfig();

		std::string	getPath() { return _path; }

	private:
		std::string											_path;
		std::string											_code;
		std::map<std::string, Server::Location>::iterator	_location;
		std::map<std::string, Server::Location>				_locationsMap;


		void	findLocation( Server * );
		void	copyLocationConfig( Server * );
		void	pathRouting();


};
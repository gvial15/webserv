#pragma once
#include <map>
#include "../Class/Request.hpp"
#include "../Class/Configuration.hpp"
#include "../Class/Server.hpp"

class RequestConfig {
	public:
		RequestConfig();
		RequestConfig( Request &, Server * );
		~RequestConfig();

		std::string	getPath() { return _path; }

	private:
		std::string	_path;

};
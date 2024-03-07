#include <exception>
#include <iostream>
#include "Class/Webserv.hpp"
#include "Class/Server.hpp"
#include "Class/Configuration.hpp"

int	main(int argc, char **argv)
{
	if (argc > 2)
		return std::cout << "Error: Webserv takes 1 or no arguments\n", 1;
	try {
		std::string config_file_path;
		if (argc == 2)
			config_file_path = argv[1];
		else
			config_file_path = "config/default.conf";
		// parse config file into a vector of servers {}
		Configuration	config(config_file_path);
		// run servers, process request and send response to client (poll(), accept(), ...)
		Webserv webserv(config.get_servers());
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
}
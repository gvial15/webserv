#include <exception>
#include <iostream>
#include "Class/Webserv.hpp"

int	main(int argc, char **argv)
{
	std::string configFile(argv[1]);

	if (argc != 2)
		std::cout << "Error: Webserv takes 2 arguments\n";	
	try {
		// parse config
		Configuration	config(configFile);
		// start server
		Webserv webserv(config);
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
}

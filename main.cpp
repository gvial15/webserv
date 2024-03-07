#include <exception>
#include <iostream>
#include "Class/Webserv.hpp"
#include "Class/Server.hpp"
#include "Class/Configuration.hpp"


// 1. 	DONE In the main() i create a Config object that takes
//		my config file as a string.

// 2. 	DONE inside the Config object i fill a vector of Server object,
//		one for each server {} block in the config, that each self
//		initialize there own socket, and bind them to there
// 		respective ip:port and make them listen() at creation.

// 3.	DONE Then once the Server vector is created and all the servers
//		fds are binded and listen(), i pass the server vector to a Webserv
//		object that fills a vector of pollfd with all the Server(s)
// 		socket and runs the poll() loop.

// 4. 	DONE then in the poll() loop all the accept() of connections and manage the deconnections as well

// 5.	A class requestResponse will take in resquests and their targeted server object (config),
//      parse them and build the answer.

// 6.	A class CGI will execute the program and return the result for requests that require it.

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
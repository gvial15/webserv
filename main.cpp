#include <exception>
#include <iostream>
#include "Class/Webserv.hpp"
#include "Class/Server.hpp"
#include "Class/Configuration.hpp"


// 1. 	In the main() i create a Config object that takes
//		my config file as a string

// 2. 	inside the Config object i fill a vector of Server object,
//		one for each server {} block in the config, that each self
//		initialize there own socket, and bind them to there
// 		respective ip:port and make them listen() at creation.

// 3.	Then once the Server vector is created and all the servers
//		fds are binded and listen(), i pass the server vector to a Webserv
//		object that fills a vector of pollfd with all the Server(s)
// 		socket and runs the poll() loop.

// 4. 	then in the poll() loop all the accept() of connections and
//		processing of request from clients is managed.

int	main(int argc, char **argv)
{
	printf("%i\n", argc);
	if (argc > 2)
		return std::cout << "Error: Webserv takes 1 or no arguments\n", 1;
	try {
		std::string configFile(argv[1]);
		// parse config file into a vector of servers {}
		Configuration	config(configFile);
		// run servers, process request and send response to client (poll(), accept(), ...)
		Webserv webserv(config.getServers());
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
}

// Server Sockets:
// Each server instance would have its own listening socket,
// which is used to accept incoming client connections.

// pollfd Structure:
// A pollfd structure would be created for each server socket,
// and these structures would be collected together in an array or vector.
// Each pollfd structure specifies which events to monitor
// on its associated socket (e.g., POLLIN for incoming data).

// Polling Loop:
// A single polling loop would call poll() to wait for events
// on any of the server sockets. The -1 timeout value in the poll()
// call means it will wait indefinitely for an event to occur.

// Event Handling:
// When poll() returns, the loop iterates through the pollfd array/vector
// to check which sockets have events. For each socket with an event,
// the appropriate action is taken, such as accepting a new client connection,
// reading incoming data, or handling disconnections.

// Client Sockets:
// When a new client connection is accepted on a server socket,
// a new pollfd structure is created for the client socket,
// and this structure is added to the pollfd array/vector,
// so that poll() will also monitor this client socket for events.

// Continued Monitoring:
// The polling loop continues monitoring all server and client sockets for events,
// and handling those events as they occur.
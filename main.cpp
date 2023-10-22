#include <exception>
#include <iostream>
#include "Class/Webserv.hpp"
#include "Class/Server.hpp"
#include "Class/Configuration.hpp"


int	main(int argc, char **argv)
{
	std::string configFile(argv[1]);

	if (argc != 2)
		std::cout << "Error: Webserv takes 2 arguments\n";
	try {
		// parse config and create a vector<Server> servers
		Configuration	config(configFile);
		// start server, polling
		Webserv webserv(config);
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
#include "../Class/Server.hpp"
#include <arpa/inet.h>  // inet_pton()
#include <fcntl.h> // fcntl() macros
#include <unistd.h> // close()

// constructor
Server::Server() :
port(8080),
ip("127.0.0.1")
{}

// destructor
Server::~Server() {
	close(server_fd);
}

// create socket, make it non-blocking, bind() and listen()
void	Server::init()
{
	struct sockaddr_in address;
	// int flags;

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw SocketException();

		// /!\ SELON LE SUJET: (aucun problème à simplement enlever ce qui à étét commenté à priori ?)
			// Toutefois, vous ne pouvez utiliser fcntl() que de la façon suivante :
			// fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
			// Tout autre flag est interdit.

	// Set the socket to non-blocking mode
	// flags = fcntl(server_fd, F_GETFL, O_NONBLOCK, FD_CLOEXEC);
	// if (flags == -1)
		// throw fcntl_F_GETFL_Exception();
	if (fcntl(server_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
		throw fcntl_F_SETFL_O_NONBLOCK_Exception();

	// setup sockaddr_in
	if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0)
		throw inet_pton_Exception();
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	// Binding the socket to the port
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw BindException();

	// Listening for a connection
	if (listen(server_fd, 500) < 0)
		throw ListenException();
}

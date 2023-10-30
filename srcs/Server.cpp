#include "../Class/Server.hpp"
#include <arpa/inet.h>  // inet_pton()
#include <fcntl.h> // fcntl() macros

// constructor
Server::Server(std::string ip, int port):
ip(ip), port(port)
{
	init_socket();
}

// destructor
Server::~Server() {}

void	Server::init_socket()
{
    struct sockaddr_in address;
	int flags;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        // create exception for this in class
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set the socket to non-blocking mode
    flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1)
    {
        // create exception for this in class
        perror("fcntl F_GETFL failed");
        exit(EXIT_FAILURE);
    }
    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        // create exception for this in class
        perror("fcntl F_SETFL O_NONBLOCK failed");
        exit(EXIT_FAILURE);
    }

    // setup sockaddr_in
    address.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0)
    {
        // create exception for this in class
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }
    address.sin_port = htons(port);

    // Binding the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        // create exception for this in class
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for a connection
    if (listen(server_fd, 3) < 0)
    {
        // create exception for this in class
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

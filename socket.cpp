#include "Socket.hpp"
#include <stdexcept>
#include <sys/socket.h>

Socket::Socket(u_long serv_ip, int port) {
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(serv_ip);
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
}

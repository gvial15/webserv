#include <sys/socket.h>
#include <netinet/in.h>

class Socket {
	public:
		Socket(u_long serv_ip, int port);
		~Socket();

	private:
		int	socket_fd;
		struct	sockaddr_in address;
};
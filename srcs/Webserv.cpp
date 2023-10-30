#include "../Class/Webserv.hpp"
#include <arpa/inet.h>
#include <unistd.h>

// constructor
Webserv::Webserv(std::vector<Server> servers) :
servers(servers)
{
    create_pollfds();
    run();
}

// destructor
Webserv::~Webserv() {};

void Webserv::run()
{
    while (true)
    {
    	size_t i = 0;
		int new_socket;
		struct sockaddr_in address;
		socklen_t addrlen = sizeof(address);
        int ret = poll(&pollfd_vec[0], pollfd_vec.size(), -1);
    
        if (ret < 0)
            throw PollException();
        while (i < pollfd_vec.size())
        {
            if (pollfd_vec[i].revents & POLLIN)  // Event on the socket
            {
                if (i < servers.size())  // Is a new connection
                {
                    if ((new_socket = accept(pollfd_vec[i].fd, (struct sockaddr *)&address, &addrlen)) < 0)
						throw AcceptException();
                    // display_socket_infos(new_socket);
					create_and_add_new_client(new_socket);
                }
                else  // Input from a client
                {
                    char buffer[1024];
                    int bytes_read = read(pollfd_vec[i].fd, buffer, 1024);
                    if (bytes_read <= 0)  // Connection closed or error
                    {
                    	close(pollfd_vec[i].fd);
                    	pollfd_vec[i].fd = 0;  // Mark as available
                    }
                    else
                    {
                    	buffer[bytes_read] = '\0';
                    	std::cout << buffer << std::endl;
                    }
                }

                // Clear the revents field for the next poll call
                pollfd_vec[i].revents = 0;
            }
            ++i;
        }
    }
}

void    Webserv::create_pollfds()
{
    std::vector<Server>::iterator it = servers.begin();
    while (it != servers.end()) {
        struct pollfd fds;  // +1 for the server socket
    	memset(&fds, 0, sizeof(fds));
    	fds.fd = it->getServerFd();
    	fds.events = POLLIN;
		pollfd_vec.push_back(fds);
        ++it;
    }
}

void    Webserv::create_and_add_new_client(int new_socket)
{
	struct pollfd client_pfd;
	memset(&client_pfd, 0, sizeof(client_pfd));
	client_pfd.fd = new_socket;
	client_pfd.events = POLLIN;
	pollfd_vec.push_back(client_pfd);
}

void    Webserv::display_socket_infos(int client_socket)
{
	struct sockaddr_in sender_address;
	socklen_t sender_addrlen = sizeof(sender_address);

	if (getpeername(client_socket, (struct sockaddr*)&sender_address, &sender_addrlen) == 0)
	{
		char client_ip[INET_ADDRSTRLEN];

		if (inet_ntop(AF_INET, &sender_address.sin_addr, client_ip, INET_ADDRSTRLEN))
			std::cout << "Connection from: " << client_ip << std::endl;
		else 
			perror("inet_ntop() failed in display_socket_infos");
	}
	else
		perror("getpeername failed");
}

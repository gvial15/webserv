#include "../Class/Webserv.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <utility>

Webserv* Webserv::instance = nullptr;

// constructor
Webserv::Webserv(std::vector<Server> servers) :
servers(servers) {
	instance = this;
	// init servers
	for (int i = 0; i < servers.size(); ++i)
		this->servers[i].init();
	create_pollfds();
	run();
}

// destructor
Webserv::~Webserv() {
	instance = nullptr;
	close_all_fds();
};

void	Webserv::signal_handler(int signum) {
	instance->close_all_fds();
	exit(signum);
}

void    Webserv::create_pollfds() {
	std::vector<Server>::iterator it = servers.begin();

	while (it != servers.end()) {
		struct pollfd fds;
		memset(&fds, 0, sizeof(fds));
		fds.fd = it->get_server_fd();
		fds.events = POLLIN;
		pollfd_vec.push_back(fds);
		++it;
	}
}

void	Webserv::run() {
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	while (true) {
		size_t i = 0;
		int client_socket;
		struct sockaddr_in address;
		socklen_t addrlen = sizeof(address);
		int ret = poll(&pollfd_vec[0], pollfd_vec.size(), -1);
	
		if (ret < 0)
			throw PollException();
		while (i < pollfd_vec.size()) {
			if (pollfd_vec[i].revents & POLLIN) { // Event on the socket
				if (i < servers.size()) { // Is a new client
					if ((client_socket = accept(pollfd_vec[i].fd, (struct sockaddr *)&address, &addrlen)) < 0)
						throw AcceptException();
					create_and_add_new_client(client_socket);
					fd_to_server_map.insert(std::make_pair(client_socket, &servers[i]));
					display_new_client_infos(client_socket, servers[i].get_port());
				}
				else { // Input from an existing client
					char buffer[1024];
					int bytes_read = read(pollfd_vec[i].fd, buffer, 1024);

					std::cout << "Input from client " << pollfd_vec[i].fd << " on port " << fd_to_server_map.find(pollfd_vec[i].fd)->second->get_port() << "\n";
					if (bytes_read <= 0) { // Connection with client closed or error
						std::cout << "closing communication with client "<< pollfd_vec[i].fd << "\n\n";
						close(pollfd_vec[i].fd); // close fd
						fd_to_server_map.erase(pollfd_vec[i].fd); // delete fd->server mapping 
						pollfd_vec[i].fd = 0;  // Mark fd as available
					}
					else {
						buffer[bytes_read] = '\0';
						// print clients request
						std::cout << buffer << std::endl;
						// write back to client
						write(pollfd_vec[i].fd, "Message received\n", 17);
					}
				}
				// Clear the revents field for the next poll call
				pollfd_vec[i].revents = 0;
			}
			++i;
		}
	}
}

void    Webserv::create_and_add_new_client(int client_socket) {
	struct pollfd client_pfd;
	memset(&client_pfd, 0, sizeof(client_pfd));
	client_pfd.fd = client_socket;
	client_pfd.events = POLLIN;
	pollfd_vec.push_back(client_pfd);
}

void    Webserv::display_new_client_infos(int client_socket, int port) {
	struct sockaddr_in sender_address;
	socklen_t sender_addrlen = sizeof(sender_address);

	if (getpeername(client_socket, (struct sockaddr*)&sender_address, &sender_addrlen) == 0) {
		char client_ip[INET_ADDRSTRLEN];

		if (inet_ntop(AF_INET, &sender_address.sin_addr, client_ip, INET_ADDRSTRLEN))
			std::cout << "New client (" << client_socket << ") connection from: " << client_ip << " on port " << port << "\n\n";
		else 
			perror("inet_ntop() failed in display_socket_infos");
	}
	else
		perror("getpeername failed");
}

void	Webserv::close_all_fds() {
	size_t i = -1;

	while (++i < pollfd_vec.size()) {
		if (pollfd_vec[i].fd >= 0) {
			close(pollfd_vec[i].fd);
			pollfd_vec[i].fd = -1;
		}
	}
}

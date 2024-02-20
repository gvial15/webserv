#include "../Class/Webserv.hpp"
#include "../Class/CGI.hpp"
#include "../Class/Request.hpp"
#include "../Class/RequestConfig.hpp"
#include "../Class/Response.hpp"
#include "../Class/ResponseHeader.hpp"
#include <arpa/inet.h>
#include <string>
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

// main server loop
void	Webserv::run() {
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	while (true) {
		size_t i = 0;
		int ret;
	
		ret = poll(&pollfd_vec[0], pollfd_vec.size(), -1);
		if (ret < 0)
			throw PollException();
		while (i < pollfd_vec.size()) {
			if (pollfd_vec[i].revents & POLLIN) { // Event on a socket
				if (i < servers.size()) // Input from new client
					add_new_client(pollfd_vec[i].fd, servers[i]);
				else // Input from existing client
					manage_client_request(pollfd_vec[i].fd);
				// Clear the revents field for the next poll call
				pollfd_vec[i].revents = 0;
			}
			++i;
		}
	}
}

// accept connection, create and add pollfd to pollfd_vec, map the fd with corresponding server/config
void	Webserv::add_new_client(int pollfd, Server &server) {
	int client_socket;
	struct pollfd client_pfd;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);

	if ((client_socket = accept(pollfd, (struct sockaddr *)&address, &addrlen)) < 0)
		throw AcceptException();
	memset(&client_pfd, 0, sizeof(client_pfd));
	client_pfd.fd = client_socket;
	client_pfd.events = POLLIN;
	pollfd_vec.push_back(client_pfd);
	fd_to_server_map.insert(std::make_pair(client_socket, &server));
	// *** testing ***
	display_new_client_infos(client_socket, server.get_port());
}

// read client input, send it to processing
void	Webserv::manage_client_request(int pollfd) {
	char buffer[1024];
	int bytes_read = read(pollfd, buffer, 1024);

	std::cout << "Input from client " << pollfd << " on port " << fd_to_server_map.find(pollfd)->second->get_port() << "\n";
	if (bytes_read <= 0) { // Connection with client closed or error
		std::cout << "closing communication with client "<< pollfd << "\n\n";
		close(pollfd);
		fd_to_server_map.erase(pollfd);
		pollfd = 0;
	}
	else { // TODO: process request (parsing, response , cgi)
		buffer[bytes_read] = '\0';
		// print clients request *** testing ***
		Request	req(buffer);
		RequestConfig requestConfig( req, fd_to_server_map.find(pollfd)->second );
		Response response;
		response.call( req, requestConfig );
		// write back to client *** testing ***
		std::string rep = response.getResponse();
		send( pollfd, rep.c_str(), rep.size(), 0 );
	}
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

// *********** testing functions ***********
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
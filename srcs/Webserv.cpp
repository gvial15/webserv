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

volatile sig_atomic_t shutdown_flag = 0;

Webserv* Webserv::instance = nullptr;

// constructor
Webserv::Webserv(std::vector<Server> servers) :
servers(servers) {
	instance = this;
	// init servers
	for (size_t i = 0; i < servers.size(); ++i)
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
	(void)	signum;
	instance->close_all_fds();
	shutdown_flag = 1;
}

void    Webserv::create_pollfds() {
	std::vector<Server>::iterator it = servers.begin();

	while (it != servers.end()) {
		struct pollfd fds;
		memset(&fds, 0, sizeof(fds));
		fds.fd = it->get_server_fd();
		fds.events = POLLIN | POLLOUT;
		pollfd_vec.push_back(fds);
		++it;
	}
}

// main server loop
void	Webserv::run() {
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	while (!shutdown_flag) {
		size_t i = 0;
		int ret;
		ret = poll(&pollfd_vec[0], pollfd_vec.size(), -1);
		if (ret < 0){
			if (shutdown_flag)
				throw SignalException();
			else
				throw PollException();
		}
		while (i < pollfd_vec.size()) {
			// check if socket is ready for READING (POLLIN)
			if (pollfd_vec[i].revents & POLLIN) { // Event on a socket
				if (i < servers.size()) // Input from new client
					add_new_client(pollfd_vec[i].fd, servers[i]);
				else // Input from existing client
					process_request(pollfd_vec[i].fd);
			}
			// check if sokcet is ready for WRITING (POLLOUT) AND if there is a pending respones associated with the client socket fd
			if ((pollfd_vec[i].revents & POLLOUT) && (this->pending_responses.find(pollfd_vec[i].fd) != this->pending_responses.end())
				&& (pending_requests[pollfd_vec[i].fd].content_length == pending_requests[pollfd_vec[i].fd].body_size))
					manage_client_response(pollfd_vec[i].fd, &i);
				// Clear the revents field for the next poll call
				pollfd_vec[i].revents = 0;
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
	client_pfd.events = POLLIN | POLLOUT;
	pollfd_vec.push_back(client_pfd);
	fd_to_server_map.insert(std::make_pair(client_socket, &server));
	// *** testing ***
	display_new_client_infos(client_socket, server.get_port());
}

// read client input, send it to processing
void Webserv::process_request(int pollfd) {
    char buffer[1024];
    int bytes_read = read(pollfd, buffer, sizeof(buffer)); // Read up to sizeof(buffer) bytes

	if (bytes_read <= 0) // Connection with client closed or error
        close_connection(pollfd);
	else {
        std::cout << "Input from client " << pollfd << " on port " << fd_to_server_map.find(pollfd)->second->get_port() << "\n";

        // Append read data to the pending request
		if (pending_requests.find(pollfd) != pending_requests.end())
            pending_requests[pollfd].request.append(buffer, bytes_read); // Append binary data
		else
            pending_requests[pollfd].request.assign(buffer, bytes_read); // Assign new data

        Request req(pending_requests[pollfd].request);

        if (req.getRequestElem().find("Content-Length") != req.getRequestElem().end())
            pending_requests[pollfd].content_length = std::stoi(req.getRequestElem().find("Content-Length")->second);
        else
            pending_requests[pollfd].content_length = 0;
        pending_requests[pollfd].body_size = req.getBodySize();

        RequestConfig requestConfig(req, fd_to_server_map.find(pollfd)->second);
        Response response;
        response.call(req, requestConfig);

        // Store client request in the pending_responses map
        std::string rep = response.getResponse();
        this->pending_responses[pollfd] = rep;
        this->bytes_sent[pollfd] = 0;
    }
}

// close connection
void	Webserv::close_connection(int pollfd) {
	std::cout << "closing communication with client "<< pollfd << "\n\n";
	close(pollfd);
	fd_to_server_map.erase(pollfd);
	this->pending_responses.erase(pollfd);
	this->pending_requests.erase(pollfd);
	this->bytes_sent.erase(pollfd);
	pollfd = 0;
}

// sends the response, or the remainder of it to the client
void	Webserv::manage_client_response(int pollfd, size_t *index){
	std::string	rep = this->pending_responses.find(pollfd)->second; // retrieve the response of the client fd in the map
	size_t		bytes = this->bytes_sent.find(pollfd)->second; // gets the amount of bytes sent of the response, acts as a checkpoint

	int ret = send(pollfd, rep.c_str() + bytes, rep.size() - bytes, 0); // retrieve the amount of bytes sent with the send() fun
	if (ret < 0)
		return;
	bytes += ret; // add the amount of bytes that was just sent via send() to the initial amount sent from previous instances
	if (bytes == rep.size()){ // check if the response has been sent entirely, deletes if yes
		this->close_connection(pollfd);
		this->pollfd_vec.erase(pollfd_vec.begin() + *index);
		(*index)--;
	}
	else // if the repsonse has not been sent entirely yet, save the amounts of bytes sent. Will act as checkpoint for the next call
		this->bytes_sent[pollfd] = bytes;
}

// close all pollfd fds
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
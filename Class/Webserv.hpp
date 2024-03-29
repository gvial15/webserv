#pragma once
#include <iostream>
#include <string>
#include <sys/poll.h>
#include "Configuration.hpp"
#include "Request.hpp"
#include "Server.hpp"

// receive the Server vector and fill a vector of pollfd
// with all the Server(s) socket and runs the poll() loop.
// All the accept() and the processing queue happen in this loop as well
class Webserv {
	public:

		Webserv(std::vector<Server> servers);
		~Webserv();

		// public attributes
		static Webserv* instance;

	private:

		struct pending_request {
			std::string	request;
			int			content_length;
			int			body_size;
		};

		// private attributes
		std::vector<Server> 	servers;
		std::vector<pollfd>		pollfd_vec;
		std::map<int, Server*>	fd_to_server_map;

		std::map<int, pending_request>	pending_requests;

		std::map<int, std::string>	pending_responses;
		std::map<int, int>			bytes_sent;

		// private methods
		void        run();
		void	    close_all_fds();
		void        create_pollfds();
		static void	signal_handler(int signum);
		void		add_new_client(int fd, Server &server);
		void		process_request(int pollfd);
		void		close_connection(int pollfd);
		void		manage_client_response(int pollfd, size_t *index);

		// *********** testing functions ***********
		void        display_new_client_infos(int client_socket, int port);

		// private exceptions
		class PollException : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "poll() failed";
			}
		};

		class AcceptException : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "accept() failed";
			}
		};

		class SignalException : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "exit on signal";
			}
		};
};

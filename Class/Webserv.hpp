#pragma once
#include <iostream>
#include <sys/poll.h>
#include "Configuration.hpp"
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

		// private attributes
		std::vector<Server> 	servers;
		std::vector<pollfd>		pollfd_vec;
		std::map<int, Server*>	fd_to_server_map;

		// private methods
		void        run();
		void	    close_all_fds();
		void        create_pollfds();
		static void signal_handler(int signum);
		void        display_new_client_infos(int client_socket, int port);
		void        create_and_add_new_client(int client_socket);

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
};

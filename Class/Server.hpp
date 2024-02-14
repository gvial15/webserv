#pragma once
#include "SharedServerDirectives.hpp"
#include <cstddef>
#include <map>
#include <string>
#include <vector>


// one for each server {} block, they each self initialize their own socket
// and bind() them to their respective ip:port and make them listen()
class Server : public SharedConfigDirectives {

	public:
		Server();
		~Server();

		// public classes
		class Location : public SharedConfigDirectives {
			public:
				Location() {};
				~Location() {}
		};

	protected:

		// protected attributes
		int								server_fd;
		int								port;
		std::string						ip;
		std::vector<std::string>		server_names;
		std::map<std::string, Location>	locations;

	public:

		// public methods
		void	init();

		// getters
		int								get_server_fd() const { return server_fd; }
		int								get_port() const { return port; }
		std::string						get_ip() const { return ip; }
		std::vector<std::string>		get_server_names() const { return server_names; }
		std::map<std::string, Location>	get_locations() const { return locations; }

		// setters
		void	set_server_fd(const int new_fd) { server_fd = new_fd; }
		void	set_port(const int new_port) { port = new_port; }
		void	set_ip(const std::string new_ip) { ip = new_ip; }
		void	set_server_name(const std::string new_server_name) { server_names.push_back(new_server_name); }
		void	set_locations(const std::pair<std::string, Location> new_location) { locations.insert(new_location); }
		void	clear_server_name() { server_names.clear(); };

	private:

		// private exceptions
		class SocketException : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "Fatal Error: socket() failed";
			}
		};

		class fcntl_F_GETFL_Exception : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "Fatal Error: fcntl() F_GETFL failed";
			}
		};

		class fcntl_F_SETFL_O_NONBLOCK_Exception : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "Fatal Error: fcntl() F_SETFL O_NONBLOCK failed";
			}
		};

		class inet_pton_Exception : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "Fatal Error: inet_pton() failed";
			}
		};

		class ListenException : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "Fatal Error: listen() failed";
			}
		};

		class BindException : public std::exception {
		public:
			virtual const char* what() const throw() {
				return "Fatal Error: bind() failed";
			}
		};
};

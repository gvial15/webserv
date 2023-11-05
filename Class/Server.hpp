#pragma once
#include "SharedConfigAttributes.hpp"
#include <cstddef>
#include <map>
#include <string>
#include <vector>


// one for each server {} block, they each self initialize their own socket,
// bind() them to their respective ip:port and make them listen().
class Server : public SharedConfigAttributes {
    public:
        Server(std::string ip, int port);
        ~Server();

        // public classes
        class Location : public SharedConfigAttributes {
			public:
				Location();
				~Location();
        };

        // public methods

        // getters
        int			get_server_fd() const { return server_fd; }
        int			get_port() const { return port; }
        std::string	get_ip() const { return ip; }

        // setters
        void		set_server_fd(int fd) { server_fd = fd; }
        void		set_port(int p) { port = p; }
        void		set_ip(const std::string &new_ip) { ip = new_ip; }

        // public attributes

    private:

        // private methods
        void	init_socket();

        // private attributes
        int			server_fd;
		int			port;
        std::string	ip;

        // private exceptions

        class SocketException : public std::exception {
        public:
            virtual const char* what() const throw() {
                return "socket() failed";
            }
        };

        class fcntl_F_GETFL_Exception : public std::exception {
        public:
            virtual const char* what() const throw() {
                return "fcntl() F_GETFL failed";
            }
        };

        class fcntl_F_SETFL_O_NONBLOCK_Exception : public std::exception {
        public:
            virtual const char* what() const throw() {
                return "fcntl() F_SETFL O_NONBLOCK failed";
            }
        };

        class inet_pton_Exception : public std::exception {
        public:
            virtual const char* what() const throw() {
                return "inet_pton() failed";
            }
        };

        class ListenException : public std::exception {
        public:
            virtual const char* what() const throw() {
                return "listen() failed";
            }
        };

        class BindException : public std::exception {
        public:
            virtual const char* what() const throw() {
                return "bind() failed";
            }
        };
};

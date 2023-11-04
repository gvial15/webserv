#pragma once
#include <cstddef>
#include <map>
#include <string>


// one for each server {} block, they each self initialize there own socket,
// and bind them to there respective ip:port and make them listen() at creation.
class Server {
    public:
        Server(std::string ip, int port);
        ~Server();

        // public methods

        // getters
        int									get_server_fd() const { return server_fd; }
        int									get_port() const { return port; }
        std::string							get_ip() const { return ip; }
        std::string							get_root() const { return root; }
        std::string							get_index() const { return index; }
        std::size_t							get_autoindex() const { return autoindex; }
        std::size_t							get_client_max_body_size() const { return client_max_body_size; }
        std::map<std::string, std::string>	get_error_pages() const { return error_pages; }

        // public attributes

    private:

        // private methods
        void	init_socket();

        // private attributes
        int									server_fd;
		int									port;
        std::string							ip;
        std::string 						root;
        std::string 						index;
        bool        						autoindex;
        std::size_t 						client_max_body_size;
        std::map<std::string, std::string>	error_pages;

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

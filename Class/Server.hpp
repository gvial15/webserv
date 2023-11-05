#pragma once
#include <cstddef>
#include <map>
#include <string>
#include <vector>


// one for each server {} block, they each self initialize their own socket,
// bind() them to their respective ip:port and make them listen().
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
        std::vector<std::string>			get_try_files() const { return try_files; }
        std::map<std::string, std::string>	get_error_pages() const { return error_pages; }
        std::size_t							get_client_max_body_size() const { return client_max_body_size; }

        // setters
        void	set_server_fd(int fd) { server_fd = fd; }
        void	set_port(int p) { port = p; }
        void	set_ip(const std::string &new_ip) { ip = new_ip; }
        void	set_root(const std::string &new_root) { root = new_root; }
        void	set_index(const std::string &new_index) { index = new_index; }
        void	set_autoindex(std::size_t new_autoindex) { autoindex = new_autoindex; }
        void	set_try_files(std::vector<std::string> new_try_files) { try_files = new_try_files; }
        void	set_error_pages(const std::map<std::string, std::string> &pages) { error_pages = pages; }
        void	set_client_max_body_size(std::size_t size) { client_max_body_size = size; }

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
		std::vector<std::string>			try_files;
        std::map<std::string, std::string>	error_pages;
        std::size_t 						client_max_body_size;


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

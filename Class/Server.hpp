#pragma once
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
        int         getServerFd() const { return server_fd; }
        int         getPort() const { return port; }
        std::string getIp() const { return ip; }

        // public attributes

    private:

        // private methods
        void	init_socket();

        // private attributes
        int         server_fd;
		int 		port;
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

#pragma once
#include <iostream>
#include <sys/poll.h>
// #include "Configuration.hpp"
#include "./Config.hpp"
#include "Server.hpp"

// receive the Server vector and fill a vector of pollfd
// with all the Server(s) socket and runs the poll() loop.
// All the accept() and the processing queue happen in this loop as well
class Webserv {
    public:
        Webserv(std::vector<Server> servers);
        ~Webserv();

        // public methods

        // public attributes
        static Webserv* instance;


    private:

        // private methods
        void        run();
        void	    close_all_fds();
        void        create_pollfds();
        static void signal_handler(int signum);
        void        display_socket_infos(int new_socket);
        void        create_and_add_new_client(int new_socket);

        // private attributes
        std::vector<pollfd> pollfd_vec;
        std::vector<Server> servers;

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

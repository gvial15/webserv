#pragma once
#include <iostream>
#include <sys/poll.h>
#include "Configuration.hpp"
#include "Server.hpp"

// receive the Server vector and fill a vector of pollfd
// with all the Server(s) socket and runs the poll() loop.
// All the accept() and queue processing happens in this loop as well
class Webserv {
    public:
        Webserv(std::vector<Server> servers);
        ~Webserv();

        // public methods

        // public attributes


    private:
        // private methods
        void    create_pollfds();
        void    run();
            
        // private attributes
        std::vector<pollfd> fds_vec;
        std::vector<Server> servers;
};

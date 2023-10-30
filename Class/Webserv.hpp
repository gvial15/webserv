#pragma once
#include <iostream>
#include <sys/poll.h>
#include "Configuration.hpp"

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
        void    create_pollfds(std::vector<Server> servers);
        void    run(std::vector<Server> servers);
            
        // private attributes
        std::vector<pollfd> fds;
};

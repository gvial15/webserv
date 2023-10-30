#include "../Class/Webserv.hpp"

// constructor
Webserv::Webserv(std::vector<Server> servers)
{
    create_pollfds(servers);
    run(servers);
}

// destructor
Webserv::~Webserv() {};

void    Webserv::run(std::vector<Server> servers)
{
    (void) servers;
}

void    Webserv::create_pollfds(std::vector<Server> servers)
{
    (void) servers;
}
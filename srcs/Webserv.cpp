#include "../Class/Webserv.hpp"

// constructor
Webserv::Webserv(std::vector<Server> servers) :
servers(servers)
{
    create_pollfds();
    run();
}

// destructor
Webserv::~Webserv() {};

void Webserv::run()
{
	
}

void    Webserv::create_pollfds()
{
    std::vector<Server>::iterator it = servers.begin();
    while (it != servers.end()) {
        struct pollfd fds;  // +1 for the server socket
    	memset(&fds, 0, sizeof(fds));
    	fds.fd = it->getServerFd();
    	fds.events = POLLIN;
		fds_vec.push_back(fds);
        ++it;
    }
}

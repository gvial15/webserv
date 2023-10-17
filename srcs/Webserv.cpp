#include "../Class/Webserv.hpp"

// constructor
Webserv::Webserv(const Configuration& config)
{
    this->run(config.servers);
}

// destructor
Webserv::~Webserv() {};

void    Webserv::run(std::vector<Server>)
{
    // event loop:
	//  - receive request
	//  - formulate response
	//  - send response
	//  - start again...
}

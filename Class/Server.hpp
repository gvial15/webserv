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

        // public attributes
        int	server_fd;

    private:

        // private methods
        void	init_server_fd();

        // private attributes
        std::string	ip;
		int 		port;
};

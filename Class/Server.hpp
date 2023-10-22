#pragma once
#include <map>
#include <string>

class Server {
public:
    Server();
    ~Server();

public:
    // public methods

    // public attributes
    std::string                         listen;
    std::map<std::string, std::string>  routes;
    std::map<std::string, std::string>  defaultErrorPages;

private:

    // private methods

    // private attributes
};

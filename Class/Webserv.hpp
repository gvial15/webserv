#include <iostream>
#include "Configuration.hpp"

class Webserv {
public:
    Webserv(const Configuration& config);
    ~Webserv();

public:

    // public methods
    void run(std::vector<Server>);

    // public attributes


private:

    // private methods
        
    // private attributes

};

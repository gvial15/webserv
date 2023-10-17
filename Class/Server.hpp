#include <map>
#include <string>

class Server {
public:
    Server();
    ~Server();

public:
    // public methods

    // public attributes

private:

    // private methods

    // private attributes
    int port;
    std::string host;
    std::map<std::string, std::string> routes;
    std::map<std::string, std::string> defaultErrorPages;
};

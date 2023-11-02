#include "../../Class/Configuration.hpp"

// constructor
Configuration::Configuration(const std::string& filePath)
{
    (void)  filePath;

    // tokenize server {} blocks
    // parse each server block individually, create a Server for each server block

    // **** hardcode servers for testing ****
    Server test_server("127.0.0.1", 8080);
    servers.push_back(test_server);
    Server test_serverr("127.0.0.1", 8081);
    servers.push_back(test_serverr);
}

// destructor
Configuration::~Configuration() {};

void Configuration::parse(const std::string& filePath)
{
    (void) filePath;
}
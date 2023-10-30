#include "../Class/Configuration.hpp"

// constructor
Configuration::Configuration(const std::string& filePath)
{
    // hardcode a server for testing
    Server test_server("127.0.0.1", 8080);
    servers.push_back(test_server);

    // print file path
    std::cout << filePath << "\n";
}

// destructor
Configuration::~Configuration() {};

void Configuration::parse(const std::string& filePath)
{
    (void) filePath;
}
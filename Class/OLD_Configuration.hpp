#pragma once
#include <iostream>
#include "Server.hpp"

// parse the config file by filling the servers vector
// each server in the vector represents a server {}; block
class Configuration {
    public:
        Configuration(const std::string& filePath);
        ~Configuration();

        // public methods

        // getters
        std::vector<Server> getServers() const { return servers; }

        // public attributes

    private:

        // private methods
        void parse(const std::string& filePath);

        // private attributes
        std::vector<Server> servers;

        // private exceptions
        class UnknownDirective : public std::exception {
        public:
            virtual const char* what() const throw() {
                return "Unknown directive in config";
            }
        };
};

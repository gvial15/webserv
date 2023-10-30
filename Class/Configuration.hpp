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
        void parse(const std::string& filePath);

        // public attributes
        std::vector<Server> servers;


    private:
        // private methods

        // private attributes

        // private exceptions
        class UnknownDirective : public std::exception {
        public:
            virtual const char* what() const throw();
        };
};

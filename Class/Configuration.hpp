#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "Server.hpp"

// parse the config file by filling the servers vector
// each server in the vector represents a server {}; block
class Configuration {


    public:

        Configuration(const std::string config_file_path);
        ~Configuration();

        // public methods

        // getters
        std::vector<Server> getServers() const { return servers; }

        // public attributes

    private:

		// private data structure
		struct location_block {
			std::vector<std::string> lines;
		};

		struct server_block {
			std::vector<std::string> lines;
			std::vector<location_block> locationBlocks;
		};

        // private methods
		void		parse(std::ifstream& config_file);
		std::string	space_out_symbols(std::string file_content);

        // private attributes
        std::vector<Server> servers;

        // private exceptions
		class UnableToOpenFile : public std::exception {
				virtual const char* what() const throw() {
					return "Error: Unable to open configuration file";
        		}
    	};

		class UnknownDirective : public std::exception {
				virtual const char* what() const throw() {
					return "Error: Unknown directive in config";
				}
        };
};
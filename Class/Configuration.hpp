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

		// private data structures
		struct location_block {
			std::vector<std::string> tokens;
		};

		struct server_block {
			std::vector<std::string> tokens;
			std::vector<location_block> locationBlocks;
		};

		// private methods
		void						parse(std::ifstream& config_file);
		std::string					space_out_symbols(std::string file_content);
		std::vector<std::string>	tokenize(std::string spaced_out_content);
		std::vector<server_block>	parse_server_blocks(std::vector<std::string> tokenized_content);

		// private attributes
		std::vector<Server> servers;

		// private exceptions
		class unable_to_open_file : public std::exception {
				virtual const char* what() const throw() {
					return "Unable to open configuration file";
				}
		};

		class unknown_directive : public std::exception {
				virtual const char* what() const throw() {
					return "Unknown directive in config";
				}
		};
};
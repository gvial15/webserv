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
		class parsing_exception : public std::exception {
		protected:
			std::string message;
			int file_line;
			std::string error_token;

		public:
			parsing_exception(const std::string& msg, int line, const std::string& token = "")
				: message(msg), file_line(line), error_token(token) {
				std::ostringstream stream;
				stream << "line " << file_line << ": " << message;
				if (!error_token.empty()) {
					stream << ": '" << error_token << "'";
				}
				message = stream.str();
			}

			virtual ~parsing_exception() throw() {}

			virtual const char* what() const throw() {
				return message.c_str();
			}
		};

		class unknown_block_type : public parsing_exception {
		public:
			unknown_block_type(const int line, const std::string& token)
				: parsing_exception("Unknown block type in config", line, token) {}
		};

		class unknown_directive : public parsing_exception {
		public:
			unknown_directive(const int line, const std::string& token)
				: parsing_exception("Unknown directive in config", line, token) {}
		};

		class unable_to_open_file : public std::exception {
				virtual const char* what() const throw() {
					return "Unable to open configuration file";
				}
		};
};
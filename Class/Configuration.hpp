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
			std::vector<location_block> location_blocks;
		};

		// private methods
		void						parse(std::ifstream& config_file);
		std::string					space_out_symbols(std::string file_content);
		std::vector<std::string>	tokenize(std::string spaced_out_content);
		std::vector<server_block>	parse_server_blocks(std::vector<std::string> tokenized_content);
		void						line_counter(std::vector<std::string> tokenized_content, size_t &i, int  &line);
		server_block				create_server_block(std::vector<std::string> tokenized_content, size_t &i, int  &line);
		void						is_valid_server_block(std::vector<std::string> tokenized_content, size_t &i, int  &line);
		location_block				create_location_block(std::vector<std::string> tokenized_content, size_t &i, int  &line);
		void						is_valid_location_block(std::vector<std::string> tokenized_content, size_t &i, int  &line);
		void						create_servers(std::vector<server_block> server_blocks);
		// ***testing***
		void						print_server_blocks(const std::vector<server_block>& servers);

		// private attributes
		std::vector<Server> servers;

		// private exceptions
		class parsing_exception : public std::exception {
			public:
				parsing_exception(const std::string& msg, int line, const std::string& token)
					: message(msg), file_line(line), error_token(token) {
					std::ostringstream stream;
					stream << "Config Error: "<< "'" << message << "'";
					stream << " '" << error_token << "'";	
					stream << " at line " << file_line;
					message = stream.str();
				}

				virtual ~parsing_exception() throw() {}
				virtual const char* what() const throw() {
					return message.c_str();
				}

			protected:
				std::string message;
				int file_line;
				std::string error_token;
		};

		class unknown_block_type : public parsing_exception {
		public:
			unknown_block_type(const int line, const std::string& token)
				: parsing_exception("Unknown block type", line, token) {}
		};

		class unknown_directive : public parsing_exception {
		public:
			unknown_directive(const int line, const std::string& token)
				: parsing_exception("Unknown directive", line, token) {}
		};

		class unexpected_token : public parsing_exception {
		public:
			unexpected_token(const int line, const std::string& token)
				: parsing_exception("Unexpected token", line, token) {}
		};

		class location_path_invalid : public parsing_exception {
		public:
			location_path_invalid(const int line, const std::string& token)
				: parsing_exception("Location path invalid", line, token) {}
		};

		class unable_to_open_file : public std::exception {
				virtual const char* what() const throw() {
					return "Unable to open configuration file";
				}
		};
};
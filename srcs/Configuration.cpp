#include "../Class/Configuration.hpp"
#include <sys/stat.h>
#include <string>

// constructor
Configuration::Configuration(const std::string config_file_path)
{
	std::ifstream config_file(config_file_path);

	if (config_file_path.empty()) {
		std::cout << "Starting with default configuration...\n";
		// generate_default_config();
	}
	else {
		if (!config_file)
			throw unable_to_open_file();
		else
			parse(config_file);
	}

	// // **** hardcode servers for testing ****
	// Server test_server("127.0.0.1", 8080);
	// servers.push_back(test_server);
	// Server test_serverr("127.0.0.1", 8081);
	// servers.push_back(test_serverr);
}

// destructor
Configuration::~Configuration() {};

// ***server_blocks testing
void Configuration::print_server_blocks(const std::vector<server_block>& servers) {
    size_t i = 0, j, k, l;
    while (i < servers.size()) {
        const server_block& server = servers[i];
        std::cout << "Server Block " << (i + 1) << ":\n";
        j = 0;
        while (j < server.tokens.size()) {
            std::cout << "Token: " << server.tokens[j++] << '\n';
        }
        k = 0;
        while (k < server.location_blocks.size()) {
            const location_block& location = server.location_blocks[k];
            std::cout << "  Location Block " << (k + 1) << ":\n";
            l = 0;
            while (l < location.tokens.size()) {
                std::cout << "    Token: " << location.tokens[l++] << '\n';
            }
            ++k;
        }
        ++i;
    }
}

// *** parsing ***
void	Configuration::parse(std::ifstream& config_file) {
	std::string					file_content((std::istreambuf_iterator<char>(config_file)), std::istreambuf_iterator<char>());
	std::string					spaced_out_content;
	std::vector<std::string>	tokenized_content;
	std::vector<server_block>	server_blocks;

	spaced_out_content = space_out_symbols(file_content);
	tokenized_content = tokenize(spaced_out_content);
	server_blocks = parse_server_blocks(tokenized_content);
	print_server_blocks(server_blocks);

	// create the vector of servers from server blocks
}

// space out special symbols } { ; \n
std::string	Configuration::space_out_symbols(std::string file_content) {
	std::string spaced_out_content;
	std::size_t i = 0;
	char c;

	while (i < file_content.size()) {
		c = file_content[i];
		if (c == ';' || c == '{' || c == '}' || c == '\n' || c == '#') {
			if (i > 0 && !isspace(file_content[i - 1]))
				spaced_out_content.push_back(' ');
			spaced_out_content.push_back(c);
			if (i < file_content.size() - 1 && !isspace(file_content[i + 1]))
				spaced_out_content.push_back(' ');
		}
		else
			spaced_out_content.push_back(c);
		i++;
	}
	return (spaced_out_content);
}

// tokenize string with white spaces as delimiter
// delimiter also includes "\n" for line # tracking for error messages and "#" for commented sections parsing
std::vector<std::string>	Configuration::tokenize(std::string spaced_out_content) {
	std::vector<std::string> tokenized_content;
	std::istringstream stream(spaced_out_content);
	std::string token;
	char c;

	while (stream.get(c)) {
		if (c == '\n') {
			if (!token.empty()) {
				tokenized_content.push_back(token);
				token.clear();
			}
			tokenized_content.push_back("\\n");
		}
		else if (isspace(c)) {
			if (!token.empty()) {
				tokenized_content.push_back(token);
				token.clear();
			}
		}
		else
			token += c;
	}
	if (!token.empty())
		tokenized_content.push_back(token);
	return (tokenized_content);
}

// loop throught tokens, for each server {} block create a server_block with nested location_block, ignore commented lines
std::vector<Configuration::server_block>	Configuration::parse_server_blocks(std::vector<std::string> tokenized_content) {
	std::vector<server_block>	server_blocks;
	int							in_server_block;
	int							server_block_i;
	int							in_location_block;
	int							location_block_i;
	int							in_comment;
	int							line;
	size_t						i;

	in_comment = 0;
	in_server_block = 0;
	in_location_block = 0;
	line = 1;
	i = -1;
	while (++i < tokenized_content.size()) {
		// count lines
		if (tokenized_content[i] == "\\n") {
			line++;
			in_comment = 0;
		}
		// ignore commented sections
		if (tokenized_content[i] == "#")
			in_comment = 1;
		if (!in_comment) {
			// block types other than 'server {}' are not authorized in the main scope of file
			if (!in_server_block && tokenized_content[i] != "\\n" && tokenized_content[i] != "server")
				throw unknown_block_type(line, tokenized_content[i]);
			// find server {} blocks start
			else if (!in_server_block && tokenized_content[i] == "server"
					&& is_valid_server_block(tokenized_content, i, line)) {
				server_block server_block;
				server_blocks.push_back(server_block);
				server_block_i = server_blocks.size() - 1;
				in_server_block = 1;
			}
			// find location {} blocks start
			else if (in_server_block && tokenized_content[i] == "location"
					&& is_valid_location_block(tokenized_content, i, line)) {
				location_block location_block;
				server_blocks[server_block_i].location_blocks.push_back(location_block);
				location_block_i = server_blocks[server_block_i].location_blocks.size() - 1;
				in_location_block = 1;
			}
			// find server {} blocks end
			else if (in_server_block && !in_location_block && tokenized_content[i] == "}")
				in_server_block = 0;
			// find location {} blocks end
			else if (in_location_block && tokenized_content[i] == "}")
				in_location_block = 0;
			// when in server block push tokens in server_block.tokens
			else if (in_server_block && !in_location_block && tokenized_content[i] != "\\n")
				server_blocks[server_block_i].tokens.push_back(tokenized_content[i]);
			// when in location block fill the location_block inside the server_block struct with tokens
			else if (in_location_block && tokenized_content[i] != "\\n")
				server_blocks[server_block_i].location_blocks[location_block_i].tokens.push_back(tokenized_content[i]);
		}
	}
	return (server_blocks);
}

// verify if server block declaration is valid
int	Configuration::is_valid_server_block(std::vector<std::string> tokenized_content, size_t &i, int  &line) {
	while (tokenized_content[++i] != "{") {
		// count lines
		if (tokenized_content[i] == "\\n")
			line++;
		else if (tokenized_content[i] != "\\n" || i == tokenized_content.size() - 1)
			throw unexpected_token(line, tokenized_content[i]);
	}
	return (1);
}

// verify if location block declaration is valid
int	Configuration::is_valid_location_block(std::vector<std::string> tokenized_content, size_t &i, int  &line) {
	struct stat buffer;
	int	path_found;

	path_found = 0;
	while (tokenized_content[++i] != "{") {
		// count lines
		if (tokenized_content[i] == "\\n")
			line++;
		else if (tokenized_content[i] != "\\n" && path_found)
			throw unexpected_token(line, tokenized_content[i]);
		else if (tokenized_content[i] != "\\n") {
			// look for the specified path and test if it exist
			if (stat(tokenized_content[i].c_str(), &buffer) != 0) // todo: verify from config path not this file
				throw location_path_invalid(line, tokenized_content[i]);
			path_found = 1;
		}
	}
	return (1);
}

void	Configuration::create_servers(std::vector<server_block> server_blocks) {
	(void)	server_blocks;
}
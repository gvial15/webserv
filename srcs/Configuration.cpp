#include "../Class/Configuration.hpp"
#include <sys/stat.h>
#include <string>
#include <vector>

// constructor
Configuration::Configuration(const std::string config_file_path)
{
	std::ifstream config_file(config_file_path);

	create_directive_bank();

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
}

// destructor
Configuration::~Configuration() {};

void	Configuration::create_directive_bank() {
	directive_bank.insert("listen");
	directive_bank.insert("server_name");
	directive_bank.insert("root");
	directive_bank.insert("index");
	directive_bank.insert("autoindex");
	directive_bank.insert("client_max_body_size");
	directive_bank.insert("error_page");
	directive_bank.insert("try_files");
	directive_bank.insert("\\n");
}

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
	std::vector<std::string>	tokenized_content;
	std::vector<server_block>	server_blocks;

	space_out_symbols(file_content);
	// std::cout << file_content << "\n";
	tokenized_content = tokenize(file_content);
	// size_t	i = -1;
	// while (++i < tokenized_content.size())
	// 	std::cout << tokenized_content[i] << "\n";
	server_blocks = parse_server_blocks(tokenized_content);
	print_server_blocks(server_blocks);

	// create the vector of servers from server blocks
}

// space out special symbols } { ; \n
void Configuration::space_out_symbols(std::string& file_content) {
    std::size_t i;

	i = 0;
    while (i < file_content.size()) {
        char c = file_content[i];
		if (c == ';' || c == '{' || c == '}' || c == '\n' || c == '#') {
            if (i > 0 && !isspace(file_content[i - 1]))
                file_content.insert(i++, " ");
            ++i;
            if (i < file_content.size() && !isspace(file_content[i]))
                file_content.insert(i++, " ");
        }
		else
            ++i;
    }
}

// tokenize string with white spaces as delimiter excluding "\n" for line # tracking in error messages
// ***non-problematic but buggy behaviour: 5 newlines tokens instead of 3 at the end***
std::vector<std::string>	Configuration::tokenize(std::string spaced_out_content) {
	std::vector<std::string> tokenized_content;
	std::string token;
	int			i;

	i = -1;
	while (spaced_out_content[++i]) {
		if (spaced_out_content[i] == '#') {
			while (spaced_out_content[++i] != '\n');
			tokenized_content.push_back("\\n");
		}
		else if (spaced_out_content[i] == '\n') {
			if (!token.empty()) {
				tokenized_content.push_back(token);
				token.clear();
			}
			tokenized_content.push_back("\\n");
		}
		else if (isspace(spaced_out_content[i])) {
			if (!token.empty()) {
				tokenized_content.push_back(token);
				token.clear();
			}
		}
		else
			token += spaced_out_content[i];
	}
	if (!token.empty())
		tokenized_content.push_back(token);
	return (tokenized_content);
}

// loop throught tokens, for each server {} block create a server_block with nested location_blocks
std::vector<Configuration::server_block>	Configuration::parse_server_blocks(std::vector<std::string> tokenized_content) {
	std::vector<server_block>	server_blocks;
	int							line;
	size_t						i;

	line = 1;
	i = -1;
	while (++i < tokenized_content.size()) {
		count_line(tokenized_content, i, line);
		// block types other than 'server {}' are not authorized in the main scope of file
		if (tokenized_content[i] != "\\n" && tokenized_content[i] != "server")
			throw unknown_block_type(line, tokenized_content[i]);
		// find server {} blocks start and create server_block struct
		else if (tokenized_content[i] == "server")
			server_blocks.push_back(create_server_block(tokenized_content, i, line));
	}
	return (server_blocks);
}

Configuration::server_block	Configuration::create_server_block(std::vector<std::string> tokenized_content, size_t &i, int  &line) {
	server_block	server_block;

	is_valid_server_block(tokenized_content, i, line);
	while (tokenized_content[++i] != "}") {
		verify_end_of_line(tokenized_content, i, line);
		count_line(tokenized_content, i, line);
		// find server {} blocks start and create location_block struct
		if (tokenized_content[i] == "location")
			server_block.location_blocks.push_back(create_location_block(tokenized_content, i, line));
		if (tokenized_content[i] != "\\n" && tokenized_content[i] != "}")
			server_block.tokens.push_back(tokenized_content[i]);
	}
	return (server_block);
}

Configuration::location_block	Configuration::create_location_block(std::vector<std::string> tokenized_content, size_t &i, int  &line) {
	location_block	location_block;

	is_valid_location_block(tokenized_content, i, line);
	while (tokenized_content[++i] != "}") {
		verify_end_of_line(tokenized_content, i, line);
		count_line(tokenized_content, i, line);
		if (tokenized_content[i] != "\\n")
			location_block.tokens.push_back(tokenized_content[i]);
	}
	return (location_block);
}

// verify if server {} block declaration is valid
void	Configuration::is_valid_server_block(std::vector<std::string> tokenized_content, size_t &i, int  &line) {
	while (tokenized_content[++i] != "{") {
		count_line(tokenized_content, i, line);
		if (tokenized_content[i] != "\\n" || i == tokenized_content.size() - 1)
			throw unexpected_token(line, tokenized_content[i]);
	}
}

// verify if location {} block declaration is valid
void	Configuration::is_valid_location_block(std::vector<std::string> tokenized_content, size_t &i, int  &line) {
	struct stat buffer;
	int	path_found;

	path_found = 0;
	while (tokenized_content[++i] != "{") {
		count_line(tokenized_content, i, line);
		if (tokenized_content[i] != "\\n" && path_found)
			throw unexpected_token(line, tokenized_content[i]);
		else if (tokenized_content[i] != "\\n") {
			if (stat(tokenized_content[i].c_str(), &buffer) != 0) // TODO: verify from config path not this file
				throw location_path_invalid(line, tokenized_content[i]);
			path_found = 1;
		}
	}
}

// verify wether end of line is ';'
void	Configuration::verify_end_of_line(std::vector<std::string> tokenized_content, size_t &i, int  &line) {
	if ( i != 0 && tokenized_content[i] == "\\n"
		&& tokenized_content[i - 1] != "{" && tokenized_content[i - 1] != "}"
		&& tokenized_content[i- 1] != "\\n" && tokenized_content[i - 1] != ";" && tokenized_content[i - 1] != " ") {
		throw end_of_line(line, "");
	}
}

void	Configuration::count_line(std::vector<std::string> tokenized_content, size_t &i, int  &line) {
	if (tokenized_content[i] == "\\n")
		line++;
}

void	Configuration::create_servers(std::vector<server_block> server_blocks) {
	(void)	server_blocks;
}
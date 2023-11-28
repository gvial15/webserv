#include "../Class/Configuration.hpp"
#include <cstddef>
#include <iterator>
#include <stddef.h>
#include <sys/stat.h>
#include <string>
#include <algorithm>
#include <utility>
#include <vector>

// constructor
Configuration::Configuration(const std::string config_file_path) {
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

// The pairs represent the number of min and max allowed arguments for a given directive; -1 = no maximum
void	Configuration::create_directive_bank() {
    directive_bank.insert(std::make_pair("listen", std::make_pair(1, 1)));
    directive_bank.insert(std::make_pair("server_name", std::make_pair(1, -1)));
	directive_bank.insert(std::make_pair("return", std::make_pair(2, 2)));
	directive_bank.insert(std::make_pair("root", std::make_pair(1, 1)));
	directive_bank.insert(std::make_pair("index", std::make_pair(1, -1)));
	directive_bank.insert(std::make_pair("autoindex", std::make_pair(1, 1)));
	directive_bank.insert(std::make_pair("client_max_body_size", std::make_pair(1, 1)));
	directive_bank.insert(std::make_pair("error_page", std::make_pair(2, -1)));
	directive_bank.insert(std::make_pair("try_files", std::make_pair(2, -1)));
	directive_bank.insert(std::make_pair("methods", std::make_pair(1, 3)));
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
            std::cout << "  Location Block " << (k + 1) << ": " << server.location_blocks[k].path << "\n";
            l = 0;
            while (l < location.tokens.size()) {
                std::cout << "    Token: " << location.tokens[l++] << '\n';
            }
            ++k;
        }
        ++i;
    }
}
// ***servers testing

template <typename C>
void	print_shared_attributes(const C &obj) {
	size_t	i;
	std::map<std::string, std::string>				error_pages;
	std::map<std::string, std::string>::iterator	it;

	std::cout << "root: " << obj.get_root() << "\n";
	std::cout << "index: " << obj.get_index() << "\n";
	std::cout << "autoindex: " << obj.get_autoindex() << "\n";
	std::cout << "redirection: " << obj.get_redirection() << "\n";
	i = -1;
	while (++i < obj.get_try_files().size())
		std::cout << "tryfiles: " << obj.get_try_files()[i] << "\n";
	error_pages = obj.get_error_pages();
	for (it = error_pages.begin(); it != error_pages.end(); ++it)
    	std::cout << "error_pages: " << it->first << " " << it->second << "\n";
	std::cout << "client_max_body_size: " << obj.get_client_max_body_size() << "\n";
}

void Configuration::print_servers(const std::vector<Server>& servers) {
	size_t	i;
	size_t	ii;
	std::map<std::string, Server::Location>	locations;
	std::map<std::string, Server::Location>::iterator	it;

	i = -1;
	while (++i < servers.size()) {
		std::cout << "\nServer:\n";
		std::cout << "port: " << servers[i].get_port() << "\n";
		std::cout << "ip: " << servers[i].get_ip() << "\n";
		std::cout << "server_name: " << servers[i].get_server_name()[0] << "\n";
		print_shared_attributes(servers[i]);
		locations = servers[i].get_locations();
		for (it = locations.begin(); it != locations.end(); ++it) {
			std::cout << "\nLocation:\n";
			print_shared_attributes(it->second);
			ii = -1;
			while (++ii < it->second.get_methods().size())
				std::cout << "methods: " << it->second.get_methods()[ii] << "\n";
			i++;
		}
	}
}

// *** parsing ***
void	Configuration::parse(std::ifstream& config_file) {
	std::string					file_content((std::istreambuf_iterator<char>(config_file)), std::istreambuf_iterator<char>());
	std::vector<std::string>	tokenized_content;
	std::vector<server_block>	server_blocks;
	size_t						i;

	space_out_symbols(file_content);
	tokenized_content = tokenize(file_content);
	// i = -1;
	// while (++i < tokenized_content.size())
	// 	std::cout << tokenized_content[i] << "\n";
	server_blocks = parse_server_blocks(tokenized_content);
	if (server_blocks.empty())
		throw no_server_blocks();
	// print_server_blocks(server_blocks);
	i = -1;
	while (++i < server_blocks.size())
		servers.push_back(create_server(server_blocks[i]));
	print_servers(servers);
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

// loop throught tokens, for each server {} blocks create a server_block struct with its nested location_blocks
std::vector<Configuration::server_block>	Configuration::parse_server_blocks(std::vector<std::string> tokenized_content) {
	std::vector<server_block>	server_blocks;
	size_t						line;
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

// when a server {} block is encountered in config file, create a server_block struct and fill it with tokens
Configuration::server_block	Configuration::create_server_block(std::vector<std::string> tokenized_content, size_t &i, size_t  &line) {
	server_block	server_block;

	is_valid_server_block(tokenized_content, i, line);
	while (tokenized_content[++i] != "}") {
		if (tokenized_content[i] == "methods")
			throw unknown_directive(line, tokenized_content[i]);
		validate_directive(tokenized_content, i, line);
		verify_end_of_line(tokenized_content, i, line);
		count_line(tokenized_content, i, line);
		// find location {} blocks start and create location_block struct
		if (tokenized_content[i] == "location")
			server_block.location_blocks.push_back(create_location_block(tokenized_content, i, line));
		if (tokenized_content[i] != "\\n" && tokenized_content[i] != "}")
			server_block.tokens.push_back(tokenized_content[i]);
	}
	return (server_block);
}

// verify if server {} block declaration is valid
void	Configuration::is_valid_server_block(std::vector<std::string> tokenized_content, size_t &i, size_t &line) {
	while (tokenized_content[++i] != "{") {
		count_line(tokenized_content, i, line);
		if (tokenized_content[i] != "\\n" || i == tokenized_content.size() - 1)
			throw unexpected_token(line, tokenized_content[i]);
	}
}

// when a location {} block is encountered in a server {} block, create a location_block struct and fill it with tokens
Configuration::location_block	Configuration::create_location_block(std::vector<std::string> tokenized_content, size_t &i, size_t &line) {
	location_block	location_block;

	is_valid_location_block(location_block, tokenized_content, i, line);
	while (tokenized_content[++i] != "}") {
		validate_directive(tokenized_content, i, line);
		verify_end_of_line(tokenized_content, i, line);
		count_line(tokenized_content, i, line);
		if (tokenized_content[i] != "\\n")
			location_block.tokens.push_back(tokenized_content[i]);
	}
	return (location_block);
}

// verify if location {} block declaration is valid and fill the location_block path attribute
void	Configuration::is_valid_location_block(Configuration::location_block &location_block, std::vector<std::string> tokenized_content, size_t &i, size_t &line) {
	struct stat	buffer;
	int	path_found;

	path_found = 0;
	while (tokenized_content[++i] != "{") {
		count_line(tokenized_content, i, line);
		if (tokenized_content[i] != "\\n" && path_found)
			throw unexpected_token(line, tokenized_content[i]);
		else if (tokenized_content[i] != "\\n") {
			if (stat(tokenized_content[i].c_str(), &buffer) != 0)
				throw location_path_invalid(line, tokenized_content[i]);
			location_block.path = tokenized_content[i];
			path_found = 1;
		}
	}
}

// verify directives synthax
void	Configuration::validate_directive(std::vector<std::string> tokenized_content, size_t &i, size_t &line) {
	// verify if there is only one directive per line
	if (tokenized_content[i] == ";")
		if (tokenized_content[i + 1] != "\\n" && tokenized_content[i + 1] != "}")
			throw multiple_directive_on_same_line(line, get_full_line(tokenized_content, i));
	// verify if token is at directive position
	if ((tokenized_content[i - 1] == "{" || tokenized_content[i - 1] == "\\n")
		&& tokenized_content[i] != "{" && tokenized_content[i] != "\\n"
		&& tokenized_content[i] != "location") {
		// validate if directive is valid
		if (directive_bank.find(tokenized_content[i]) == directive_bank.end())
			throw unknown_directive(line, tokenized_content[i]);
		else {
			if (count_directive_args(tokenized_content, i) == 0)
				throw no_directive_arg(line, tokenized_content[i]);
			if (count_directive_args(tokenized_content, i) > directive_bank.find(tokenized_content[i])->second.second
				&& directive_bank.find(tokenized_content[i])->second.second != -1)
				throw to_many_directive_args(line, tokenized_content[i]);
			if (count_directive_args(tokenized_content, i) < directive_bank.find(tokenized_content[i])->second.first)
				throw not_enough_arguments_for_directive(line, tokenized_content[i]);
		}
	}
}

// count the number of arguments a directive has
int	Configuration::count_directive_args(std::vector<std::string> tokens, size_t i) {
	int	nbr_arg;

	nbr_arg = i + 1;
	while (++i < tokens.size())
		if (tokens[i] == ";")
			break;
	nbr_arg = i - nbr_arg;
	return (nbr_arg);
}

// verify wether end of line is ';'
void	Configuration::verify_end_of_line(std::vector<std::string> tokenized_content, size_t &i, size_t &line) {
	if (tokenized_content[i] == "\\n"
		&& tokenized_content[i - 1] != "{" && tokenized_content[i - 1] != "}"
		&& tokenized_content[i- 1] != "\\n" && tokenized_content[i - 1] != ";") {
		throw end_of_line(line, get_full_line(tokenized_content, i));
	}
}

std::string	Configuration::get_full_line(std::vector<std::string> tokenized_content, size_t &i) {
	std::string	line;

	i--;
	while (tokenized_content[i] != "\\n")
		i--;
	while (tokenized_content[++i] != "\\n") {
		line.insert(line.size(), tokenized_content[i]);
		if (tokenized_content[i] != ";")
			line.push_back(' ');
	}
	line.pop_back();
	return (line);
}

void	Configuration::count_line(std::vector<std::string> tokenized_content, size_t &i, size_t &line) {
	if (tokenized_content[i] == "\\n")
		line++;
}

// *** server only ***
// std::string							ip;
// int									port;

// *** shared
// std::string 							root;
// std::string 							index;
// bool 								autoindex;
// std::string							redirection;
// std::vector<std::string> 			try_files;
// std::map<std::string, std::string>	error_pages;
// std::size_t							client_max_body_size;
// int									server_fd;
// std::string							server_name;

// *** location only ***
// std::map<std::string, Location>		locations;
// std::vector<std::string>				methods;

// for each server_block create a Server object, fill it's attributes while validating the format of directives arguments
Server	Configuration::create_server(server_block server_blocks) {
	Server	server;
	size_t	i;

	// fill server attributes
	fill_server_attributes(server_blocks.tokens, server);
	i = -1;
	// fill locations attributes
	while (++i < server_blocks.location_blocks.size()) {
		Server::Location location;
		server.set_locations(std::make_pair(server_blocks.location_blocks[i].path, location));
		fill_shared_attributes(server_blocks.location_blocks[i].tokens,
		server.get_locations().find(server_blocks.location_blocks[i].path)->second);
		fill_location_attributes(server_blocks.location_blocks[i].tokens,
		server.get_locations().find(server_blocks.location_blocks[i].path)->second);
	}
	return (server);
}

void	Configuration::fill_server_attributes(std::vector<std::string> tokens, Server &server) {
	(void) server;
	std::vector<std::string>	arguments;
	size_t	i;

	i = -1;
	while (++i < tokens.size()) {
		if (i == 0 || tokens[i - 1] == ";") {
			arguments = get_arguments(tokens, i);
			if (tokens[i] == "listen") {
				// validate arguments format
				// push in the appropriate server/location attribute
			}
			else if (tokens[i] == "server_name") {
				
			}
			arguments.clear();
		}
	}
}

template <typename T>
void	Configuration::fill_shared_attributes(std::vector<std::string> tokens, T &obj) {
	(void)	obj;
	std::vector<std::string>	arguments;
	size_t	i;

	i = -1;
	while (++i < tokens.size()) {
		if (i == 0 || tokens[i - 1] == ";") {
			arguments = get_arguments(tokens, i);
			if (tokens[i] == "root") {

			}
			else if (tokens[i] == "index") {

			}
			else if (tokens[i] == "autoindex") {

			}
			else if (tokens[i] == "redirection") {

			}
			else if (tokens[i] == "try_files") {

			}
			else if (tokens[i] == "error_pages") {

			}
			else if (tokens[i] == "client_max_body_size") {

			}
			else if (tokens[i] == "methods") {
				
			}
			arguments.clear();
		}
	}
}

void	Configuration::fill_location_attributes(std::vector<std::string> tokens, Server::Location &location) {
	(void)	location;
	std::vector<std::string>	arguments;
	size_t	i;

	i = -1;
	while (++i < tokens.size()) {
		if (i == 0 || tokens[i - 1] == ";") {
			arguments = get_arguments(tokens, i);
			if (tokens[i] == "methods") {
				
			}
			arguments.clear();
		}
	}
}

std::vector<std::string>	Configuration::get_arguments(std::vector<std::string> tokens, size_t &i) {
	std::vector<std::string>	arguments;

	while (tokens[++i] != ";")
		arguments.push_back(tokens[i]);
	return (arguments);
}

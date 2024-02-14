#include "../Class/Configuration.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
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

	if (config_file_path.empty()) {
		std::cout << "Starting with default configuration...\n";
		Server	server;
		servers.push_back(server);
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

// The pairs values represent the number of min and max allowed arguments for a given directive; -1 = no maximum
void	Configuration::create_directive_bank() {
	directive_bank.insert(std::make_pair("listen", std::make_pair(1, 1)));
	directive_bank.insert(std::make_pair("server_name", std::make_pair(1, -1)));
	directive_bank.insert(std::make_pair("redirection", std::make_pair(2, 2)));
	directive_bank.insert(std::make_pair("root", std::make_pair(1, 1)));
	directive_bank.insert(std::make_pair("index", std::make_pair(1, -1)));
	directive_bank.insert(std::make_pair("autoindex", std::make_pair(1, 1)));
	directive_bank.insert(std::make_pair("client_max_body_size", std::make_pair(1, 1)));
	directive_bank.insert(std::make_pair("error_page", std::make_pair(2, 2)));
	directive_bank.insert(std::make_pair("methods", std::make_pair(1, 3)));
	directive_bank.insert(std::make_pair("post_path", std::make_pair(1, 1)));
}

// main parsing function
void	Configuration::parse(std::ifstream& config_file) {
	std::string					file_content((std::istreambuf_iterator<char>(config_file)), std::istreambuf_iterator<char>());
	std::vector<token>			tokens;
	std::vector<server_block>	server_blocks;

	create_directive_bank();
	space_out_symbols(file_content);
	tokens = tokenize(file_content);
	// size_t	i;
	// i = -1;
	// while (++i < tokens.size())
	// 	std::cout << tokens[i].line << " " << tokens[i].content << "\n";
	// std::cout << "\n\n";
	server_blocks = parse_server_blocks(tokens);
	if (server_blocks.empty())
		throw no_server_blocks();
	// print_server_blocks(server_blocks);
	size_t	i;
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

// tokenize file
std::vector<Configuration::token>	Configuration::tokenize(std::string spaced_out_content) {
	std::vector<token>	tokens;
	token				token;
	size_t				line;
	int					i;

	line = 1;
	i = -1;
	while (spaced_out_content[++i]) {
		// if char is a comment
		if (spaced_out_content[i] == '#') {
			while (spaced_out_content[++i] != '\n');
			line++;
		}
		// if char is a newline
		else if (spaced_out_content[i] == '\n')
			line++;
		// if char is a white space
		else if (isspace(spaced_out_content[i])) {
			if (!token.content.empty()) {
				token.line = line;
				tokens.push_back(token);
				token.content.clear();
			}
		}
		// fill token.content with chars
		else
			token.content += spaced_out_content[i];
	}
	if (!token.content.empty()) {
		token.line = line;
		tokens.push_back(token);
	}
	return (tokens);
}

// loop throught tokens, for each server {} blocks create a server_block struct
std::vector<Configuration::server_block>	Configuration::parse_server_blocks(std::vector<token> tokens) {
	std::vector<server_block>	server_blocks;
	size_t						i;

	i = -1;
	while (++i < tokens.size()) {
		// block types other than 'server {}' are not authorized in the main scope of file
		if (tokens[i].content != "server")
			throw unknown_block_type(tokens[i].line, tokens[i].content);
		// find server {} blocks start and create server_block struct
		else if (tokens[i].content == "server")
			server_blocks.push_back(create_server_block(tokens, i));
	}
	return (server_blocks);
}

// create a server_block struct while validating the synthax
Configuration::server_block	Configuration::create_server_block(std::vector<token> tokens, size_t &i) {
	server_block	server_block;

	is_valid_server_block(tokens, i);
	while (tokens[++i].content != "}") {
		verify_end_of_line(tokens, i);
		validate_directive(tokens, i);
		// find location {} blocks start and create location_block struct
		if (tokens[i].content == "location")
			server_block.location_blocks.push_back(create_location_block(tokens, i));
		server_block.tokens.push_back(tokens[i]);
	}
	return (server_block);
}

// verify if server {} block declaration is valid
void	Configuration::is_valid_server_block(std::vector<token> tokens, size_t &i) {
	if (tokens[i + 1].content != "{")
		throw unexpected_token(tokens[i].line, tokens[i].content);
}

// when a location {} block is encountered in a server {} block, create a location_block struct and fill it with tokens
Configuration::location_block	Configuration::create_location_block(std::vector<token> tokens, size_t &i) {
	location_block	location_block;

	is_valid_location_block(location_block, tokens, i);
	while (tokens[++i].content != "}") {
		verify_end_of_line(tokens, i);
		validate_directive(tokens, i);
		location_block.tokens.push_back(tokens[i]);
	}
	return (location_block);
}

// verify if location {} block declaration is valid and fill the location_block path attribute
void	Configuration::is_valid_location_block(Configuration::location_block &location_block, std::vector<token> tokens, size_t &i) {
	int	path_found;

	path_found = 0;
	while (tokens[++i].content != "{") {
		if (path_found)
			throw unexpected_token(tokens[i].line, tokens[i].content);
		if (!is_valid_path(tokens[i].content))
			throw location_path_invalid(tokens[i].line, tokens[i].content);
		path_found = 1;
		location_block.path = tokens[i].content;
	}
	if (!path_found)
		throw no_location_path(tokens[i].line, get_full_line(tokens, i));
}

// verify directives synthax
void	Configuration::validate_directive(std::vector<token> tokens, size_t &i) {
	// verify if there is only one directive per line
	if (tokens[i].content == ";" && tokens[i + 1].line == tokens[i].line && tokens[i + 1].content != "}")
		throw multiple_directive_on_same_line(tokens[i].line, get_full_line(tokens, i));
	// verify if token is at directive position
	if ((tokens[i - 1].content == "{" || tokens[i - 1].content == ";")
		&& tokens[i].content != "location") {
		// validate if directive is valid
		if (directive_bank.find(tokens[i].content) == directive_bank.end())
			throw unknown_directive(tokens[i].line, tokens[i].content);
		else {
			if (count_directive_args(tokens, i) == 0)
				throw no_directive_arg(tokens[i].line, tokens[i].content);
			if (count_directive_args(tokens, i) > directive_bank.find(tokens[i].content)->second.second
				&& directive_bank.find(tokens[i].content)->second.second != -1)
				throw to_many_directive_args(tokens[i].line, tokens[i].content);
			if (count_directive_args(tokens, i) < directive_bank.find(tokens[i].content)->second.first)
				throw not_enough_arguments_for_directive(tokens[i].line, tokens[i].content);
		}
	}
}

// count the number of arguments a directive has
int	Configuration::count_directive_args(std::vector<token> tokens, size_t i) {
	int	nbr_arg;

	nbr_arg = i + 1;
	while (++i < tokens.size())
		if (tokens[i].content == ";" || tokens[i].line != tokens[i - 1].line)
			break;
	nbr_arg = i - nbr_arg;
	return (nbr_arg);
}

// verify wether end of line is ';'
void	Configuration::verify_end_of_line(std::vector<token> tokens, size_t &i) {
	if (tokens[i].line != tokens[i + 1].line && tokens[i].content != "{" && tokens[i].content != ";")
		throw end_of_line(tokens[i].line, get_full_line(tokens, i));
}

// for each server_block create a Server object, fill it's attributes while validating the format of directives arguments
Server	Configuration::create_server(server_block server_blocks) {
	Server	server;
	size_t	i;

	fill_server_attributes(server_blocks.tokens, server);
	fill_shared_attributes(server_blocks.tokens, server);
	i = -1;
	while (++i < server_blocks.location_blocks.size()) {
		Server::Location location;
		fill_shared_attributes(server_blocks.location_blocks[i].tokens, location);
		server.set_locations(std::make_pair(server_blocks.location_blocks[i].path, location));
	}
	return (server);
}

// fill Server only attributes
void	Configuration::fill_server_attributes(std::vector<token> tokens, Server &server) {
	size_t						i;
	std::vector<std::string>	arguments;
	std::vector<std::string>	split_arg;

	i = -1;
	while (++i < tokens.size() && tokens[i].content != "}") {
		if (i == 0 || tokens[i - 1].content == ";" || tokens[i - 1].content == "{") {
			arguments = get_arguments(tokens, i);
			if (tokens[i].content == "listen") {
				validate_listen_arguments(tokens, i, arguments);
				split_arg = split(arguments[0], ':');
				if (split_arg.size() == 2) {
					server.set_ip(split_arg[0]);
					server.set_port(std::stoi(split_arg[1]));
				}
				else
					server.set_port(std::stoi(split_arg[0]));
			}
			else if (tokens[i].content == "server_name") {
				server.clear_server_name();
				for (int ii = 0; ii < arguments.size(); ++ii)
					server.set_server_name(arguments[ii]);
			}
			arguments.clear();
		}
	}
}

// validate format of listen directive arguments
void	Configuration::validate_listen_arguments(std::vector<token> tokens, size_t i, std::vector<std::string> arguments) {
	std::vector<std::string>	split_arg;

	split_arg = split(arguments[0], ':');
	if (split_arg.size() == 2) {
		if (!is_valid_ip(split_arg[0]))
			throw invalid_ip(tokens[i].line, split_arg[0]);
		if (!is_string_integer(split_arg[1]))
			throw invalid_port(tokens[i].line, split_arg[1]);
		if (std::stoi(split_arg[1]) < 0 || std::stoi(split_arg[1]) > 65535)
			throw invalid_port(tokens[i].line, split_arg[1]);
	}
	else {
		if (!is_string_integer(split_arg[0]))
			throw invalid_port(tokens[i].line, split_arg[0]);
		if (std::stoi(split_arg[0]) < 0 || std::stoi(split_arg[0]) > 65535)
			throw invalid_port(tokens[i].line, split_arg[0]);
	}
}

// is string an ip
bool	Configuration::is_valid_ip(const std::string& ip_address) {
	std::istringstream	ip(ip_address);
	std::string			digit;
	int					num;
	int 				num_count;

	num_count = 0;
	while (std::getline(ip, digit, '.')) {
		num = 0;
		std::istringstream	digit_stream(digit);
		if (!(digit_stream >> num) || num < 0 || num > 255)
			return (false);
		num_count++;
	}
	return (num_count == 4);
}

// fill attributes shared by both Server and Location
template <typename T>
void	Configuration::fill_shared_attributes(std::vector<token> tokens, T &obj) {
	std::vector<std::string>	arguments;
	size_t						i;

	i = -1;
	while (++i < tokens.size() && tokens[i].content != "}") {
		if (i == 0 || tokens[i - 1].content == ";") {
			arguments = get_arguments(tokens, i);
			if (tokens[i].content == "root") {
				if (!is_valid_path(arguments[0]))
					throw invalid_root_path(tokens[i].line, arguments[0]);
				obj.set_root(arguments[0]);
			}
			else if (tokens[i].content == "index") {
				obj.clear_index();
				for (int ii = 0; ii < arguments.size(); ++ii)
					obj.set_index(arguments[ii]);
			}
			else if (tokens[i].content == "autoindex") {
				if (arguments[0] == "on")
					obj.set_autoindex(true);
				else if (arguments[0] == "off")
					obj.set_autoindex(false);
				else
				 	throw invalid_autoindex_argument(tokens[i].line, arguments[0]);
			}
			else if (tokens[i].content == "redirection") {
				if (obj.get_redirection().first.empty() && obj.get_redirection().second.empty()) {
					if (!is_string_integer(arguments[0]))
						throw invalid_redirection_argument(tokens[i].line, arguments[0]);
					obj.set_redirection(std::make_pair(arguments[0], arguments[1]));
				}
			}
			else if (tokens[i].content == "error_page") {
				if (!is_string_integer(arguments[0]))
					throw invalid_error_page_argument(tokens[i].line, arguments[0]);
				if (!is_valid_path(arguments[1]))
					throw invalid_error_page_argument(tokens[i].line, arguments[1]);
				if (obj.get_error_pages().find(arguments[0]) != obj.get_error_pages().end())
					obj.erase_error_page(arguments[0]);
				obj.set_error_page(std::make_pair(arguments[0], arguments[1]));
			}
			else if (tokens[i].content == "client_max_body_size") {
				if (!is_string_size_t(arguments[0]))
					throw invalid_client_max_body_size_argument(tokens[i].line, arguments[0]);
				obj.set_client_max_body_size(string_to_size_t(arguments[0]));
			}
			else if (tokens[i].content == "methods") {
				obj.clear_methods();
				for (int ii = 0; ii < arguments.size(); ++ii) {
					if (arguments[ii] != "POST" && arguments[ii] != "GET" && arguments[ii] != "DELETE")
						throw invalid_method_argument(tokens[i].line, arguments[ii]);
					obj.set_methods(arguments[ii]);
				}
			}
			else if (tokens[i].content == "post_path") {
				if (!is_valid_path(arguments[0]))
					throw invalid_post_path_argument(tokens[i].line, arguments[0]);
				obj.set_post_path(arguments[0]);
			}
			arguments.clear();
		}
	}
}

// get the full line in which a token is
std::string	Configuration::get_full_line(std::vector<token> tokens, size_t &i) {
	std::string	line;
	size_t		initial_line_number;

	initial_line_number = tokens[i].line;
	i--;
	while (tokens[i].line == initial_line_number)
		i--;
	while (tokens[++i].line == initial_line_number) {
		line.insert(line.size(), tokens[i].content);
		if (tokens[i].line == tokens[i + 1].line && tokens[i + 1].content != ";")
			line.push_back(' ');
	}
	return (line);
}

// get directive arguments
std::vector<std::string>	Configuration::get_arguments(std::vector<token> tokens, size_t i) {
	std::vector<std::string>	arguments;

	while (tokens[++i].content != ";")
		arguments.push_back(tokens[i].content);
	return (arguments);
}

// split a string by a delimiter
std::vector<std::string>	Configuration::split(std::string string, char delimiter) {
    std::vector<std::string> elements;
    std::stringstream ss(string);
    std::string element;

    while (std::getline(ss, element, delimiter))
        elements.push_back(element);

    return (elements);
}

// is string integer
bool Configuration::is_string_integer(const std::string &string) {
    std::istringstream ss(string);
    int num;

    ss >> num;
    return (!ss.fail() && ss.eof());
}

// is string size_t
bool Configuration::is_string_size_t(const std::string &string) {
    std::istringstream iss(string);
    size_t num;

    iss >> num;
    return (!iss.fail() && iss.eof());
}

// string to size_t
size_t Configuration::string_to_size_t(const std::string &string) {
    std::istringstream iss(string);
    size_t num;

    iss >> num;
    return (num);
}

// verify if path exist
bool	Configuration::is_valid_path(std::string path) {
	struct stat	buffer;
	if (stat(path.c_str(), &buffer) != 0)
		return (false);
	return (true);
}

// *********** testing functions ***********
void Configuration::print_server_blocks(const std::vector<server_block> &servers) {
	size_t i = 0, j, k, l;
	while (i < servers.size()) {
		const server_block& server = servers[i];
		std::cout << "Server Block " << (i + 1) << ":\n";
		j = 0;
		while (j < server.tokens.size()) {
			std::cout << "Token: " << server.tokens[j++].content << '\n';
		}
		k = 0;
		while (k < server.location_blocks.size()) {
			const location_block& location = server.location_blocks[k];
			std::cout << "  Location Block " << (k + 1) << ": " << server.location_blocks[k].path << "\n";
			l = 0;
			while (l < location.tokens.size()) {
				std::cout << "    Token: " << location.tokens[l++].content << '\n';
			}
			++k;
		}
		++i;
	}
}

template <typename C>
void	print_shared_attributes(const C &obj) {
	size_t	i;
	std::map<std::string, std::string>				error_pages;
	std::map<std::string, std::string>::iterator	it;

	std::cout << "root: " << obj.get_root() << "\n";
	i = -1;
	std::cout << "index: ";
	while (++i < obj.get_index().size())
		std::cout << obj.get_index()[i] << " ";
	std::cout << "\n";
	std::cout << "methods: ";
	i = -1;
	while (++i < obj.get_methods().size())
		std::cout << obj.get_methods()[i] << " ";
	std::cout << "\n";
	std::cout << "post_path: " << obj.get_post_path() << "\n";
	std::cout << "autoindex: " << obj.get_autoindex() << "\n";
	std::cout << "redirection: " << obj.get_redirection().first << " " << obj.get_redirection().second << "\n";
	error_pages = obj.get_error_pages();
	for (it = error_pages.begin(); it != error_pages.end(); ++it)
		std::cout << "error_pages: " << it->first << " " << it->second << "\n";
	std::cout << "client_max_body_size: " << obj.get_client_max_body_size() << "\n";
}

void Configuration::print_servers(const std::vector<Server> &servers) {
	size_t	i;
	size_t	ii;
	std::map<std::string, Server::Location>	locations;
	std::map<std::string, Server::Location>::iterator	it;

	i = -1;
	while (++i < servers.size()) {
		std::cout << "\nServer:\n";
		std::cout << "port: " << servers[i].get_port() << "\n";
		std::cout << "ip: " << servers[i].get_ip() << "\n";
		std::cout << "server_name: ";
		for (int s = 0; s < servers[i].get_server_names().size(); ++s)
			std::cout << servers[i].get_server_names()[s] << " ";
		std::cout << "\n";
		print_shared_attributes(servers[i]);
		locations = servers[i].get_locations();
		for (it = locations.begin(); it != locations.end(); ++it) {
			std::cout << "\nLocation " << it->first << "\n";
			print_shared_attributes(it->second);
			std::cout << "\n";
		}
	}
}
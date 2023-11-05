#include "../Class/Configuration.hpp"

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

    // **** hardcode servers for testing ****
    Server test_server("127.0.0.1", 8080);
    servers.push_back(test_server);
    Server test_serverr("127.0.0.1", 8081);
    servers.push_back(test_serverr);
}

// destructor
Configuration::~Configuration() {};

// *** parsing ***
void	Configuration::parse(std::ifstream& config_file)
{
	std::string					file_content((std::istreambuf_iterator<char>(config_file)), std::istreambuf_iterator<char>());
	std::string					spaced_out_content;
	std::vector<std::string>	tokenized_content;
	std::vector<server_block>	server_blocks;


	spaced_out_content = space_out_symbols(file_content);
	tokenized_content = tokenize(spaced_out_content);
	// std::size_t i = -1;
	// while (++i < tokenized_content.size())
	// 	std::cout << tokenized_content[i] << "\n";

	// loop throught tokens and for each server {} block create a server_block

	// create the vector of servers from server blocks
}

// space out special symbols } { ; \n
std::string Configuration::space_out_symbols(std::string file_content) {
    std::string spaced_out_content;
    std::size_t i = 0;
    char c;

    while (i < file_content.size()) {
    	c = file_content[i];
		if (c == ';' || c == '{' || c == '}' || c == '\n') {
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

// tokenize string with white spaces as delimiter but include "\n" as token for line # tracking for error messages
std::vector<std::string>	Configuration::tokenize(std::string spaced_out_content)
{
    std::istringstream			stream(spaced_out_content);
	std::vector<std::string>	tokenized_content;
	std::string					token;

    while (stream >> token) {
        tokenized_content.push_back(token);
        if (stream.peek() == '\n') {
            stream.get();
            tokenized_content.push_back("\n");
        }
    }
	return (tokenized_content);
}

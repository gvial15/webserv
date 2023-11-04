#include "../Class/Configuration.hpp"
#include <cctype>
#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

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
			throw UnableToOpenFile();
		else {
			parse(config_file);
		}
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
	std::vector<server_block>	server_blocks;


	// space out special symbols
	spaced_out_content = space_out_symbols(file_content);
	std::cout << spaced_out_content << "\n";
	// tokenize string \n included to track line # for error messages

	// loop throught tokens and for each server {} block
	// create a ServerBlock
}

std::string	Configuration::space_out_symbols(std::string file_content)
{
	std::string	spaced_out_content;
	std::size_t	i;
	char		c;

	i = -1;
	while (++i < file_content.size()) {
		c = file_content[i];
		if ((c == ';' || c == '{' || c == '}')
			&& i != 0 && i != file_content.size() - 1) {
			if (!isspace(file_content[i + 1]) && !isspace(file_content[i - 1])) {
				spaced_out_content.push_back(' ');
				spaced_out_content.push_back(c);
				spaced_out_content.push_back(' ');
			}
			else if (!isspace(file_content[i - 1])) {
				spaced_out_content.push_back(' ');
				spaced_out_content.push_back(c);
			}
			else if (!isspace(file_content[i + 1])) {
				spaced_out_content.push_back(c);
				spaced_out_content.push_back(' ');
			}
			else
				spaced_out_content.push_back(c);
		}
		else
			spaced_out_content.push_back(c);
	}
	return (spaced_out_content);
}

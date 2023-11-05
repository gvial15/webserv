#include <string>
#include <vector>
#include <map>
#include <cstddef> // for std::size_t

class SharedConfigAttributes {

	public:
		SharedConfigAttributes() :
			root("/var/www/html"), // Default root
			index("index.html"),   // Default index page
			autoindex(false),      // Autoindex off by default
			client_max_body_size(1 << 20) // Default max body size, for example, 1MB
		{}

		// public methods

		// setters
		void set_root(const std::string &new_root) { root = new_root; }
		void set_index(const std::string &new_index) { index = new_index; }
		void set_autoindex(bool new_autoindex) { autoindex = new_autoindex; }
		void set_try_files(const std::vector<std::string> &new_try_files) { try_files = new_try_files; }
		void set_error_pages(const std::map<std::string, std::string> &new_error_pages) { error_pages = new_error_pages; }
		void set_client_max_body_size(std::size_t new_size) { client_max_body_size = new_size; }

		// getters
		std::string get_root() const { return root; }
		std::string get_index() const { return index; }
		bool get_autoindex() const { return autoindex; }
		std::vector<std::string> get_try_files() const { return try_files; }
		std::map<std::string, std::string> get_error_pages() const { return error_pages; }
		std::size_t get_client_max_body_size() const { return client_max_body_size; }

	protected:
		std::string root;
		std::string index;
		bool autoindex;
		std::vector<std::string> try_files;
		std::map<std::string, std::string> error_pages;
		std::size_t client_max_body_size;
};

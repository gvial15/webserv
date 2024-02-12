#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cstddef>

// Class from which Server and Location inherit containing all their shared attributes
class SharedConfigAttributes {

	protected:

		// protected attributes
		std::string 						root;
		std::vector<std::string> 			index;
		bool 								autoindex;
		std::pair<std::string, std::string>	redirection;
		std::vector<std::string>			try_files;
		std::map<std::string, std::string>	error_pages;
		std::size_t							client_max_body_size;

	public:

		SharedConfigAttributes() :
			root("/html"),
			autoindex(false),
			client_max_body_size(1 << 20)
		{
			index.push_back("index.html");
			error_pages.insert(std::make_pair("401", "/html/401_unauthorized.html"));
			error_pages.insert(std::make_pair("404", "/html/404_not_found.html"));
			error_pages.insert(std::make_pair("500", "/html/500_internal_server_error.html"));
			error_pages.insert(std::make_pair("502", "/html/502_bad_gateway.html"));
			error_pages.insert(std::make_pair("503", "/html/503_service_unavailable.html"));
			error_pages.insert(std::make_pair("504", "/html/504_gateway_timeout.html"));
		}
		SharedConfigAttributes( SharedConfigAttributes const & src ) { *this = src; }

		SharedConfigAttributes &	operator=( SharedConfigAttributes const & src ) {
			root = src.root;
			index = src.index;
			autoindex = src.autoindex;
			redirection = src.redirection;
			try_files = src.try_files;
			error_pages = src.error_pages;
			client_max_body_size = src.client_max_body_size;
			return *this;
		}

		// getters
		std::string 						get_root() const { return root; }
		std::vector<std::string> 			get_index() const { return index; }
		bool								get_autoindex() const { return autoindex; }
		std::pair<std::string, std::string>	get_redirection() const { return redirection; }
		std::vector<std::string>			get_try_files() const { return try_files; }
		std::map<std::string, std::string>	get_error_pages() const { return error_pages; }
		std::size_t 						get_client_max_body_size() const { return client_max_body_size; }

		// setters
		void 	set_root(const std::string new_root) { root = new_root; }
		void 	set_index(const std::string new_index) { index.push_back(new_index); }
		void	set_autoindex(const bool new_autoindex) { autoindex = new_autoindex; }
		void 	set_redirection(const std::pair<std::string, std::string> new_redirection) { redirection = new_redirection; }
		void 	set_try_files(const std::string new_try_files) { try_files.push_back(new_try_files); }
		void 	set_error_page(const std::pair<std::string, std::string> new_error_pages) { error_pages.insert(new_error_pages); }
		void 	set_client_max_body_size(const std::size_t new_client_max_body_size) { client_max_body_size = new_client_max_body_size; }

		void	clear_index() { index.clear(); }
		void	clear_try_files() { try_files.clear(); }
		void	erase_error_page(std::string key) { error_pages.erase(key); }

};

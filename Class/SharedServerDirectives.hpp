#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cstddef>

// Class from which Server and Location inherit containing all their shared attributes
class SharedConfigDirectives {

	protected:

		// protected attributes
		std::string 						root;
		std::vector<std::string> 			index;
		std::vector<std::string>			methods;
		std::string 						post_path;
		bool 								autoindex;
		std::pair<std::string, std::string>	redirection;
		std::map<std::string, std::string>	error_pages;
		std::size_t							client_max_body_size;
		std::vector<std::string>			cgi_ext;

	public:

		SharedConfigDirectives() :
			root("/html"),
			post_path("/default/path"), // to be changed
			autoindex(false),
			client_max_body_size(1 << 20)
		{
			methods.push_back("GET");
			methods.push_back("POST");
			methods.push_back("DELETE");
			// index.push_back("index.html");
			// error_pages.insert(std::make_pair("401", "/html/401_unauthorized.html"));
			// error_pages.insert(std::make_pair("404", "/html/404_not_found.html"));
			// error_pages.insert(std::make_pair("500", "/html/500_internal_server_error.html"));
			// error_pages.insert(std::make_pair("502", "/html/502_bad_gateway.html"));
			// error_pages.insert(std::make_pair("503", "/html/503_service_unavailable.html"));
			// error_pages.insert(std::make_pair("504", "/html/504_gateway_timeout.html"));
		}
		SharedConfigDirectives( SharedConfigDirectives const & src ) { *this = src; }

		SharedConfigDirectives &	operator=( SharedConfigDirectives const & src ) {
			root = src.root;
			index = src.index;
			methods = src.methods;
			post_path = src.post_path;
			autoindex = src.autoindex;
			redirection = src.redirection;
			error_pages = src.error_pages;
			client_max_body_size = src.client_max_body_size;
			cgi_ext = src.cgi_ext;
			return *this;
		}


		// getters
		std::string 						get_root() const { return root; }
		std::vector<std::string> 			get_index() const { return index; }
		std::vector<std::string>			get_methods() const { return methods; }
		std::string							get_post_path() const { return post_path; }
		bool								get_autoindex() const { return autoindex; }
		std::pair<std::string, std::string>	get_redirection() const { return redirection; }
		std::map<std::string, std::string>	get_error_pages() const { return error_pages; }
		std::size_t 						get_client_max_body_size() const { return client_max_body_size; }
		std::vector<std::string>			get_cgi_ext() const { return cgi_ext; }

		// setters
		void 	set_root(const std::string new_root) { root = new_root; }
		void 	set_index(const std::string new_index) { index.push_back(new_index); }
		void	set_autoindex(const bool new_autoindex) { autoindex = new_autoindex; }
		void	set_methods(std::string new_methods) { methods.push_back(new_methods); }
		void	set_post_path(std::string new_post_path) { post_path = new_post_path; }
		void 	set_redirection(const std::pair<std::string, std::string> new_redirection) { redirection = new_redirection; }
		void 	set_error_page(const std::pair<std::string, std::string> new_error_pages) { error_pages.insert(new_error_pages); }
		void 	set_client_max_body_size(const std::size_t new_client_max_body_size) { client_max_body_size = new_client_max_body_size; }
		void	set_cgi_ext(std::string new_cgi_ext) {cgi_ext.push_back(new_cgi_ext); }

		void	clear_index() { index.clear(); }
		void	clear_methods() { methods.clear(); };
		void	erase_error_page(std::string key) { error_pages.erase(key); }

};

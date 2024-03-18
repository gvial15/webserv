#include <map>
#include <algorithm>
#include "../Class/RequestConfig.hpp"
#include "../Class/Server.hpp"

#include <sys/stat.h>

struct both_slashes {
    bool operator()(char a, char b) const {
        return a == '/' && b == '/';
    }
};

RequestConfig::RequestConfig() {}

RequestConfig::RequestConfig( Request & request, Server * server ) : SharedConfigDirectives( *server ) {
	// insert path if not exist in copied server // DEFAULT
	error_pages.insert(std::make_pair("401", "error_pages/401_unauthorized.html"));
	error_pages.insert(std::make_pair("404", "error_pages/404_not_found.html"));
	error_pages.insert(std::make_pair("405", "error_pages/405_method_not_allowed.html"));
	error_pages.insert(std::make_pair("413", "error_pages/413_content_too_large.html"));
	error_pages.insert(std::make_pair("500", "error_pages/500_internal_server_error.html"));
	error_pages.insert(std::make_pair("502", "error_pages/502_bad_gateway.html"));
	error_pages.insert(std::make_pair("503", "error_pages/503_service_unavailable.html"));
	error_pages.insert(std::make_pair("504", "error_pages/504_gateway_timeout.html"));

	_host = server->get_ip();
	_port = server->get_port();

	if ( request.getCode() == 400 )
		return;

	// GET PATH DEPENDING ON LOCATION / ROOTS / INDEXES
	_path = const_cast<std::map<std::string, std::string>&>(request.getRequestElem())["path"];
	_locationsMap = server->get_locations();

	this->findLocation();
	this->copyLocationConfig();
	this->pathRouting();

	// need redirection ?

	
	_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );

}

RequestConfig::~RequestConfig() {}

// Update _location var
void	RequestConfig::findLocation() {
	std::string								truncated_path = _path;
	if ( truncated_path.back() != '/' )
		truncated_path.push_back('/');

	// check if part of a location
	while ( truncated_path.size() > 0 ) {
		_location = _locationsMap.begin();
		while( _location != _locationsMap.end() ) {
			std::string location_path = _location->first;
			if ( location_path[0] != '/' )
				location_path = "/" + location_path;

			if ( truncated_path == location_path )
				return;
			_location++;
		}
		truncated_path.pop_back();
	}
	//  << "No location found " << std::endl;
}

// if _location is valid, copy values and truncate path
void	RequestConfig::copyLocationConfig() {
	if ( _location == _locationsMap.end() )
		return;

	// SET VALUES FROM LOCATION more tbd
	if ( _location->second.get_root() != "" )
		set_root( _location->second.get_root() );
	if ( _location->second.get_index().size() > 0 )
		index = _location->second.get_index();
	if ( _location->second.get_methods().size() > 0 )
		methods = _location->second.get_methods();
	if ( _location->second.get_autoindex() )
		set_autoindex( _location->second.get_autoindex() );
	if ( _location->second.get_redirection().first != "" )
		set_redirection( _location->second.get_redirection() );
	if ( _location->second.get_cgi_ext().size() > 0)
		cgi_ext = _location->second.get_cgi_ext();

	std::map<std::string, std::string> location_error_pages = _location->second.get_error_pages();
	std::map<std::string, std::string>::iterator it;
	for ( it = location_error_pages.begin(); it != location_error_pages.end(); it++ ) {
		error_pages.find( it->first )->second = it->second;
	}
	
	if ( _location->second.get_client_max_body_size() != 1 << 20 && _location->second.get_client_max_body_size() > 0 )
		set_client_max_body_size( _location->second.get_client_max_body_size() );

	// remove associated location path
	std::string location_minus_slash = _location->first;
	if ( location_minus_slash.back() == '/' )
		location_minus_slash.pop_back();
	std::string::size_type i = _path.find(location_minus_slash);
	if (i != std::string::npos)
		_path.erase(i, location_minus_slash.length());
}

// check indexes and append root
void	RequestConfig::pathRouting() {	
	_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
	if ( _path == "" || _path == "/" ) {
		std::vector<std::string>::iterator it = index.begin();
		// Check every Index
		while ( it != index.end() ) {
			_path = this->get_root() + "/" + *it;
			_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
			struct stat s;
			if ( stat( _path.c_str(), &s ) == 0 ) {
				if ( s.st_mode & S_IFREG ) {
					break;
				} //else
					// std::cout << _path << " is not a file\n";
			} //else
				// File doesnt exist or ERROR 505
				// std::cout << errno << " - " << _path << " - ERROR 505\n";
			_path = this->get_root() + "/";
			it++;
		}
		if ( _path == "" ) { // NO INDEX FOUND
			return;
		}
	}
	else
		_path = this->get_root() + _path;
}

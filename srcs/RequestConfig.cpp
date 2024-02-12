#include <map>
#include "../Class/RequestConfig.hpp"
#include "../Class/Server.hpp"

#include <sys/stat.h>

struct both_slashes {
    bool operator()(char a, char b) const {
        return a == '/' && b == '/';
    }
};

RequestConfig::RequestConfig() {}

RequestConfig::RequestConfig( Request & request, Server * server ) : SharedConfigAttributes( *server ) {

				std::cout << "-1" << std::endl;

	//parse query string


	// GET PATH DEPENDING ON LOCATION / ROOTS / INDEXES
	_path = const_cast<std::map<std::string, std::string>&>(request.getRequestElem())["path"];
	_locationsMap = server->get_locations();

	this->findLocation( server );
	this->copyLocationConfig( server );
	this->pathRouting();

	// if location found
	// if ( _location != server->get_locations().end() ) {


	// 	_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
	// 	if ( _path == "" || _path == "/" ) {
	// 		std::vector<std::string>::iterator it = _location->second.get_index().begin();
	// 		while ( it != _location->second.get_index().end() ) {
	// 			_path = _location->second.get_root() + "/" + *it; // WIP need to check every index
	// 			_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
	// 			struct stat s;
	// 			if ( stat( _path.c_str(), &s ) == 0 ) {
	// 				if ( s.st_mode & S_IFREG ) {
	// 					std::cout << _path << " is a file it seems\n";
	// 					break;
	// 				} else
	// 					std::cout << _path << " is not a file\n";
	// 			} else
	// 				// ERROR 505
	// 				std::cout << errno << " - " << _path << " - ERROR 505\n";
	// 			_path = "";
	// 			it++;
	// 		}
	// 		if ( _path == "" ) { // NO INDEX FOUND
	// 			std::cout << "ERROR no correct index found\n";
	// 			return;
	// 		}
	// 	}
	// 	else
	// 		_path = _location->second.get_root() + _path;

	// 	// std::cout << "I'm im location: " << location_minus_slash << " - root: " << _location->second.get_root() << " !\n";
	// }
	// else {

	// 	_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
	// 	if ( _path == "" || _path == "/" )
	// 		_path = server->get_root() + "/" + server->get_index()[0]; // WIP need to check every index
	// 	else
	// 		_path = server->get_root() + _path;

	// 	std::cout << "I'm im server root: " << server->get_root() << " !\n";
	// }

	std::cout << "path result: " << _path << std::endl;
	
	_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
	std::cout << "erase result: " << _path << std::endl;

	std::cout << "path is: " << _path << std::endl; //debug
}

RequestConfig::~RequestConfig() {}

void	RequestConfig::findLocation( Server * server ) {
	std::string								truncated_path = _path;
	if ( truncated_path.back() != '/' )
		truncated_path.push_back('/');

	// check if part of a location
	size_t	cmp_size = truncated_path.size();
	while ( truncated_path.size() > 0 ) {
		_location = _locationsMap.begin();
		while( _location != _locationsMap.end() ) {
			std::string location_path = _location->first;
			if ( location_path[0] != '/' )
				location_path = "/" + location_path;

			if ( truncated_path == location_path ) {
				std::cout << "FOUND " << std::endl;
				std::cout << location_path << std::endl;
				return;
			}
			_location++;
		}
		truncated_path.pop_back();
	}
	std::cout << "No location found " << std::endl;
}

void	RequestConfig::copyLocationConfig( Server * server ) {
		if ( _location == _locationsMap.end() )
			return;

		// SET VALUES FROM LOCATION
		if ( _location->second.get_root() != "" )
			set_root( _location->second.get_root() );
		if ( _location->second.get_index().size() > 0 )
			index = _location->second.get_index();
		if ( _location->second.get_autoindex() )
			set_autoindex( _location->second.get_autoindex() );
		if ( _location->second.get_redirection().first != "" )
			set_redirection( _location->second.get_redirection() );
		if ( _location->second.get_try_files().size() > 0 )
			try_files = _location->second.get_try_files();
		if ( _location->second.get_error_pages().size() > 0 )
			error_pages = _location->second.get_error_pages();
		if ( _location->second.get_client_max_body_size() )
			set_client_max_body_size( _location->second.get_client_max_body_size() );

		std::cout << "location name: " << _location->first << std::endl;
		std::cout << "location root: " << _location->second.get_root() << std::endl;
		std::cout << "location index size: " << _location->second.get_index().size() << std::endl;
		std::cout << "location autoindex: " << _location->second.get_autoindex() << std::endl;
		std::cout << "location redirection 1: " << _location->second.get_redirection().first << std::endl;
		std::cout << "location get_try_files().size(): " << _location->second.get_try_files().size() << std::endl;
		std::cout << "location get_error_pages().size(): " << _location->second.get_error_pages().begin()->second << std::endl; // SEGFAULT TBD
		std::cout << "location get_client_max_body_size(): " << _location->second.get_client_max_body_size() << std::endl;


		std::string location_minus_slash = _location->first;
		if ( location_minus_slash.back() == '/' )
			location_minus_slash.pop_back();

		std::string::size_type i = _path.find(location_minus_slash);
		if (i != std::string::npos)
			_path.erase(i, location_minus_slash.length());
}

void	RequestConfig::pathRouting() {	
	_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
	if ( _path == "" || _path == "/" ) {
		std::vector<std::string>::iterator it = index.begin();
		while ( it != index.end() ) {
			_path = this->get_root() + "/" + *it; // WIP need to check every index
			_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
			struct stat s;
			if ( stat( _path.c_str(), &s ) == 0 ) {
				if ( s.st_mode & S_IFREG ) {
					std::cout << _path << " is a file it seems\n";
					break;
				} else
					std::cout << _path << " is not a file\n";
			} else
				// ERROR 505
				std::cout << errno << " - " << _path << " - ERROR 505\n";
			_path = "";
			it++;
		}
		if ( _path == "" ) { // NO INDEX FOUND
			std::cout << "ERROR no correct index found\n";
			return;
		}
	}
	else
		_path = this->get_root() + _path;
}

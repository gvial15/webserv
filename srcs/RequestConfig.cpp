#include <map>
#include "../Class/RequestConfig.hpp"
#include "../Class/Server.hpp"

struct both_slashes {
    bool operator()(char a, char b) const {
        return a == '/' && b == '/';
    }
};

RequestConfig::RequestConfig() {}

RequestConfig::RequestConfig( Request & request, Server * server ) {
	// GET PATH DEPENDING ON LOCATION / ROOTS / INDEXES
	_path = const_cast<std::map<std::string, std::string>&>(request.getRequestElem())["path"];

	std::string truncated_path = _path;
	if ( truncated_path.back() != '/' )
		truncated_path.push_back('/');

	std::map<std::string, Server::Location> 			locations = server->get_locations();
	std::map<std::string, Server::Location>::iterator	location_it;

	// check if part of a location
	while ( truncated_path.size() > 0 ) {
		location_it = locations.begin();
		while( location_it != locations.end() ) {
			std::string location_path = location_it->first;
			if ( location_path[0] != '/' )
				location_path = "/" + location_path;
			if ( truncated_path == location_path )
				break;
			location_it++;
		}
		if ( location_it != locations.end() )
			break;
		truncated_path = truncated_path.substr( 0, truncated_path.size() - 1 );
	}

	// if location found
	if ( location_it != locations.end() ) {
		std::string location_minus_slash = location_it->first;
		if ( location_minus_slash.back() == '/' )
			location_minus_slash.pop_back();

		std::string::size_type i = _path.find(location_minus_slash);
		if (i != std::string::npos)
			_path.erase(i, location_minus_slash.length());

		_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
		if ( _path == "" || _path == "/" )
			_path = location_it->second.get_root() + "/" + location_it->second.get_index()[0]; // WIP need to check every index
		else
			_path = location_it->second.get_root() + _path;

		std::cout << "I'm im location: " << location_minus_slash << " - root: " << location_it->second.get_root() << " !\n";
	}
	else {

		_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
		if ( _path == "" || _path == "/" )
			_path = server->get_root() + "/" + server->get_index()[0]; // WIP need to check every index
		else
			_path = server->get_root() + _path;

		std::cout << "I'm im server root: " << server->get_root() << " !\n";
	}

	std::cout << "path result: " << _path << std::endl;
	
	_path.erase( std::unique(_path.begin(), _path.end(), both_slashes()), _path.end() );
	std::cout << "erase result: " << _path << std::endl;

	std::cout << "path is: " << _path << std::endl; //debug
}

RequestConfig::~RequestConfig() {}
		
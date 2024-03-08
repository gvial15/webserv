#pragma once

#include "Request.hpp"
#include "RequestConfig.hpp"
#include <unistd.h>
#include <vector>

class CGI {

    private:

        std::string _scriptPath;
        std::string _postData;
        std::string _query;
        std::string _response;
		std::string	_method;
        std::string _contentType;
        std::string _pathInfo;
        std::string _execLocation;
        std::string _script;
		int			_status;

    public:

        CGI( Request & request, RequestConfig &config );

        std::string	executeCgiScript( void );
        void        childProcess( int* stdout_pipefd, int* stdin_pipefd );
        std::string vector_to_string(std::vector<std::string> vector);
        void        splitScriptPath( void );

		std::string const &getScriptPath( void ) const;
		std::string const &getPostData( void ) const;
		std::string const &getResponse( void ) const;
		int               getStatus( void ) const;
};
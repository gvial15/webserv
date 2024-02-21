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
		int			_status;

    public:

        CGI( Request & request, RequestConfig &config );

        std::string	executeCgiScript( void );

		std::string const &getScriptPath( void ) const;
		std::string const &getPostData( void ) const;
		std::string const &getResponse( void ) const;
		int			const getStatus( void ) const;

		void		setEnvp( std::vector<char *> &envp );
};
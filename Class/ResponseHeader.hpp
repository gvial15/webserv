#pragma once

#include "Request.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

class Request;

class ResponseHeader {

	private:

		std::map<std::string, std::string>	_httpStatusMap;
		std::string							_responseHeader;
		std::string							_statusLine;
		std::string							_HTTPVersion;
		std::string 						_statusCode;
		std::string 						_reasonPhrase;
		std::string							_date;
		std::string							_server;
		std::string							_lastModified;
		std::string							_contentLength;
		std::string							_contentType;
		std::string							_connection;

	public:
		ResponseHeader(int status, const char *path, int size, const char *extension);
		~ResponseHeader();

		// create Response
		void			createHttpStatusMap( void );
		void			createResponseHeader( void );
		void			createStatusLine( void );

		//initialize values from values sent from the Request process
		void			initializeValues( int Status, const char *PATH, int Size, const char *extension);
		std::string		initCurrentDateTimeGMT( void );
		std::string		initLastModified(const char* path);
		std::string		getMimeType(const char* extension);


		// utils & debug
		std::string		structToTime(const tm * time);
		std::string		nbToString( int n );

		// getters

		const std::string &getResponseHeader() const;
		const std::string &getStatusLine() const;
		const std::string &getHTTPVersion() const;
		const std::string &getStatusCode() const;
		const std::string &getReasonPhrase() const;
		const std::string &getDate() const;
		const std::string &getServer() const;
		const std::string &getLastModified() const;
		const std::string &getContentLength() const;
		const std::string &getContentType() const;
		const std::string &getConnection() const;
};

// HTTP/1.1 200 OK
// Date: Mon, 27 Jul 2020 12:28:53 GMT
// Server: Apache/2.4.1 (Unix)
// Last-Modified: Wed, 22 Jul 2020 19:15:56 GMT
// Content-Length: 88
// Content-Type: text/html
// Connection: Closed
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arivera <arivera@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 12:43:17 by arivera           #+#    #+#             */
/*   Updated: 2024/02/06 14:07:43 by arivera          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Class/Request.hpp"

Request::Request( const char *request ): _request(request){
    this->parse();
}

void    Request::printRequestElems() const{
    for (std::map<std::string, std::string>::const_iterator it = this->_requestElem.begin(); it != _requestElem.end(); ++it) {
        std::cout << it->first << " -> " << it->second << std::endl;
    }
}

void Request::stringToLower(std::string &str) {
    for (size_t i = 0; i < str.length(); ++i) {
        str[i] = std::tolower(static_cast<unsigned char>(str[i]));
    }
}

// Parsing

void    Request::parseFirstLine(std::string firstLine){
    std::istringstream fl(firstLine);
    std::string method, path, protocol;
    fl >> method;
    fl >> path;
    fl >> protocol;
    this->_requestElem.insert(std::make_pair("method",method));
    this->_requestElem.insert(std::make_pair("path",path));
    this->_requestElem.insert(std::make_pair("protocol",protocol));
}

void Request::parseHeaders(std::istringstream& iss) {
    std::string line;
    while (std::getline(iss, line)) {
        std::size_t pos = line.find(":");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            this->stringToLower(key);
            this->_requestElem[key] = value;
        }
    }
}

void    Request::parse( void ){
    std::istringstream iss(_request);
    std::string firstLine;
    std::getline(iss, firstLine);
    std::cout << firstLine << std::endl;
    this->parseFirstLine(firstLine);
    this->parseHeaders(iss);
    
    printRequestElems();
}

// Getters

const std::string                           &Request::getRequest( void ) const{
    return this->_request;
}

const std::map<std::string, std::string>    &Request::getRequestElem( void ) const{
    return this->_requestElem;
}

// GET / HTTP/1.1
// Host: localhost:8081
// Connection: keep-alive
// Cache-Control: max-age=0
// sec-ch-ua: "Not.A/Brand";v="8", "Chromium";v="114", "Google Chrome";v="114"
// sec-ch-ua-mobile: ?0
// sec-ch-ua-platform: "macOS"
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9
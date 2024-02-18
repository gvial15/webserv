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
#include "../Class/Configuration.hpp"
#include <codecvt>
#include <iostream>
#include <string>
#include <sys/_types/_size_t.h>
#include <vector>

Request::Request( const char *request ): _request(request) {
    parse();
}

void    Request::printRequestElems() const {
    for (std::map<std::string, std::string>::const_iterator it = _requestElem.begin(); it != _requestElem.end(); ++it)
        std::cout << it->first << " -> " << it->second << std::endl;
}


// Parsing
void	Request::parseFirstLine(std::string first_line) {
	std::vector<std::string>	split_line;

	split_line = split(first_line, ' ');
    if (split_line.size() != 3)
        std::cout << "400 bad request (missing method/path/protocol)\n"; // 400 bad request;
	if (split_line[0] != "GET" && split_line[0] != "POST" && split_line[0] != "DELETE")
		std::cout << "400 bad request (method)\n"; // 400 bad request;
	if (split_line[2] != "HTTP/1.1\r")
		std::cout << "400 bad request2 (protocol)\n"; // 400 bad request;
	_requestElem["method"] = split_line[0];
	_requestElem["path"] = split_line[1];
	_requestElem["protocol"] = split_line[2];
}

void	Request::parseHeaders(std::vector<std::string> lines) {
	size_t	i;
	std::vector<std::string>	split_line;
	std::string					substr;

	i = 0;
	while (++i < lines.size() - 1 && lines[i] != "\r\n") {
		split_line = split(lines[i], ':');
		if (split_line.size() > 1) {
			substr = lines[i].substr(split_line[0].size() + 1, lines[i].size() - split_line[0].size() - 2);
			_requestElem[split_line[0]] = substr;
		}
	}
}

void	Request::parse() {
    std::vector<std::string>    lines;

	// std::cout << "\n*****request parsing*****\n\n";
	// std::cout << _request << "\n\n"; // print request
    lines = split(_request, '\n');
	parseFirstLine(lines[0]);;
	parseHeaders(lines);
	// std::cout << "\nrequestElems:\n";
    // printRequestElems();
	// std::cout << "\n*****request parsing END*****\n\n";
}

// utils functions
void Request::stringToLower(std::string &str) {
    for (size_t i = 0; i < str.length(); ++i)
        str[i] = std::tolower(static_cast<unsigned char>(str[i]));
}

std::vector<std::string>	Request::split(std::string string, char delimiter) {
    std::vector<std::string> elements;
    std::stringstream ss(string);
    std::string element;

    while (std::getline(ss, element, delimiter))
        elements.push_back(element);
    return (elements);
}

// Getters
const std::string                           &Request::getRequest() const { return (_request); }

const std::map<std::string, std::string>    &Request::getRequestElem() const { return (_requestElem); }

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
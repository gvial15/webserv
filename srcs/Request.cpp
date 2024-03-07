/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arivera <arivera@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 12:43:17 by arivera           #+#    #+#             */
/*   Updated: 2024/02/21 16:01:19 by arivera          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Class/Request.hpp"
#include "../Class/Configuration.hpp"
#include <_stdio.h>
#include <codecvt>
#include <cstddef>
#include <iostream>
#include <string>
#include <sys/_types/_size_t.h>
#include <vector>

Request::Request( std::string request ): _request(request) {
    parse();
}

void    Request::printRequestElems() const {
	std::cout << "\n";
    for (std::map<std::string, std::string>::const_iterator it = _requestElem.begin(); it != _requestElem.end(); ++it)
        std::cout << it->first << " -> " << it->second << std::endl;
}

std::string	Request::find_filname(std::vector<std::string>, int i) {
	int			ii;
	std::string	filename;

	while (split(_body[++i], ':')[0] != "Content-Disposition");
	filename = remove_char(remove_char(split(split(_body[i], ';')[2], '=')[1], '"'), '\r');
	return (filename);
}

void Request::parseFiles() {
	std::string filename;

	for (int i = 0; i < _body.size(); ++i) {
		if (_body[i] == _boundary) {
			filename = find_filname(_body, i);
			_filenames.push_back(filename);
			while (endsWith(_body[++i], "\r"));
		}
		if (_body[i] != remove_char(_boundary, '\r') + "--\r")
			_files[filename] = _files[filename] + _body[i] + "\n";
	}
}

void	Request::parseBody(std::vector<std::string> &lines, size_t i) {
	--i;
	while (++i < lines.size())
		if (lines[i] != "\r")
			_body.push_back(lines[i]);
}

void	Request::parseHeaders(std::string &lines) {
	std::vector<std::string>	split_line;
	std::string					substr;

	split_line = split(lines, ':');
	if (split_line.size() > 1) {
		substr = lines.substr(split_line[0].size() + 1, lines.size() - split_line[0].size() - 2);
		_requestElem[split_line[0]] = substr;
		if (split_line[0] == "Content-Type")
			_boundary = "--" + split(split_line[1], '=')[1];
		if (_boundary == "--")
			_boundary.clear();
	}
}

// Parsing
void	Request::parseFirstLine(std::string first_line) {
	size_t						i;
	std::vector<std::string>	split_line;
	std::vector<std::string>	split_path;

	split_line = split(first_line, ' ');
	_code = 200;
    if ((split_line.size() != 3) ||
		(split_line[0] != "GET" && split_line[0] != "POST" && split_line[0] != "DELETE") ||
		(split_line[2] != "HTTP/1.1\r")) {
		_code = 400;
		return ;
	}
	// get query or path_info
	_requestElem["path"] = split_line[1];
	split_path = split(split_line[1], '?');
	if (split_path.size() == 2) {
		_requestElem["path"] = split_path[0];
		_query = split_path[1];
	}
	else {
		split_path = split(split_line[1], '/');
		i = -1;
		while (++i < split_path.size()) {
			if (i == 0)
				_requestElem["path"].erase();
			_requestElem["path"] = _requestElem["path"] + split_path[i] + "/";
			if (endsWith(split_path[i], ".py") || endsWith(split_path[i], ".sh")) {
				_path_info = "/";
				while (++i < split_path.size())
					_path_info = _path_info + split_path[i] + "/";
				_path_info.pop_back();
			}
		}
		_requestElem["path"].pop_back();
	}
	_requestElem["method"] = split_line[0];
	_requestElem["protocol"] = split_line[2];
}

void	Request::parse() {
	size_t						i;
    std::vector<std::string>    lines;

    lines = split(_request, '\n');
	parseFirstLine(lines[0]);
	if (_code == 400)
		return ;
	i = 0;
	while (++i < lines.size() - 1 && lines[i] != "\r\n" && lines[i] != "\r\n\r\n"
			&& lines[i] != "\r" && lines[i] != "\n")
				parseHeaders(lines[i]);
	parseBody(lines, i);
	if (_boundary.size() != 0)
		parseFiles();
}

// utils functions
std::string Request::remove_char(std::string input, char c) {
    std::string result;

    for (size_t i = 0; i < input.length(); ++i)
        if (input[i] != c)
            result += input[i];
    return (result);
}

bool Request::endsWith(const std::string str, const std::string suffix) {
    if (str.length() >= suffix.length())
        return (str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0);
    else
        return false;
}

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
const int	Request::getBodySize() const {
	int	size = 0;

	for (int i = 0; i < _body.size(); i++)
		if (_boundary.empty())
			size += _body[i].size();
		else
			size += _body[i].size() + 1;
	if (!_boundary.empty())
		size += 2;
	return (size);
};

const std::string                           &Request::getRequest() const { return (_request); }
const std::map<std::string, std::string>    &Request::getRequestElem() const { return (_requestElem); }
const std::vector<std::string>				&Request::getBody() const { return (_body); }
std::vector<std::string>					&Request::getFilenames() { return (_filenames); }
std::map<std::string, std::string>			&Request::getFiles() { return (_files); }
const std::string                           &Request::getQuery( void ) const {return (_query); }
const int									&Request::getCode( void ) const { return (_code); }
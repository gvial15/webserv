/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arivera <arivera@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 12:43:19 by arivera           #+#    #+#             */
/*   Updated: 2024/02/06 14:00:11 by arivera          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cstddef>
#include <iostream>
#include <map>
#include <sstream>
#include <cctype>
#include <string>

class Request {
    
    private:

        const std::string                   _request;
        std::map<std::string, std::string>  _requestElem;
        std::string                         _body;

    public:

		// Constructor
        Request(const char *req);

		// Parsing functions
        void    parse();
        void    parseFirstLine(std::string first_line);
        void    parseHeaders(std::string &lines);
        void    parseBody(std::vector<std::string> &lines, size_t i);

		// utils & debug
        std::vector<std::string>	split(std::string string, char delimiter);
		void	                    stringToLower(std::string &str);
		void                        printRequestElems() const;
        
        //getters
        const std::string                           &getRequest( void ) const;
        const std::map<std::string, std::string>    &getRequestElem( void ) const;
        const std::string                           &getBody( void ) const;

};
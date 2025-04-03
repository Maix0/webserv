/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:48:32 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 13:57:32 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>

#include "app/http/Request.hpp"
#include "app/http/Response.hpp"

std::string Response::createStatusPageFor(StatusCode code) {
	std::stringstream req;
	std::stringstream body;

	std::string canonical = code.canonical().get_or("Unknown Code");

	req << "HTTP/1.1 " << code.code() << " " << canonical << CRLF;
	req << "Content-Type: text/html; charset=UTF-8" CRLF;
	req << "Content-Length: ";

	body << "<html>" CRLF;
	body << "<head><title> " << code.code() << " " << canonical << " "
		 << "</title></head>" CRLF;
	body << "<body>" CRLF;
	body << "<center><h1>" << code.code() << " " << canonical << " "
		 << "</h1></center>" CRLF;

	std::string						 body_str = body.str();
	req << body_str.length() << CRLF CRLF << body_str;

	return req.str();
}

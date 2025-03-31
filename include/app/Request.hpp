/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:51:48 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/31 17:52:08 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <cstdio>
#include <exception>
#include <sstream>
#include <string>
#include <vector>
#include "app/IndexMap.hpp"

#define CLRF "\r\n"

class Request {
	public:
		typedef IndexMap<std::string, std::string> HeaderMap;
		typedef std::string						   Url;
		typedef std::string						   Method;

		static const std::vector<std::string> ALLOWED_MULTIHEADERS;

		enum ParsingState { UNDEFINED, HEADER, USERHEADERS, BODY, FINISHED };

	private:
		HeaderMap	 headers;
		Url			 url;
		Method		 method;
		ParsingState state;
		std::string	 tmp_buffer;

		// POST DATA FOR BODIES
		int		body_fd;
		size_t	body_size;
		ssize_t content_length;

	public:
		const HeaderMap& getHeaders() const { return this->headers; };
		const Url&		 getUrl() const { return this->url; };
		const Method&	 getMethod() const { return this->method; };

		HeaderMap&	 getHeaders() { return this->headers; };
		Url&		 getUrl() { return this->url; };
		Method&		 getMethod() { return this->method; };
		ParsingState getState() { return this->state; };

		void parseBytes(std::vector<char>::const_iterator start,
						std::vector<char>::const_iterator end);

		Request() : state(HEADER) {};
		~Request() {
			if (this->body_fd != -1)
				close(this->body_fd);
		};

		class PageException : public std::exception {
			private:
				std::string str;
				int			code;

			public:
				virtual ~PageException() throw() {}
				PageException(int code) {
					std::stringstream ss;
					ss << "requested Page for status code " << code;
					this->str = ss.str();
				}
				virtual const char* what(void) const throw() { return this->str.c_str(); }
				int					statusCode() { return this->code; };
		};
};

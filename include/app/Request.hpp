/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:51:48 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/29 17:54:15 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include "app/IndexMap.hpp"
#include "app/Option.hpp"
#include "app/Shared.hpp"

#define CLRF "\r\n"

class Request {
	public:
		typedef IndexMap<std::string, std::string> HeaderMap;
		typedef std::string						   Url;
		typedef std::string						   Method;
		typedef std::string						   Body;

		static const std::vector<std::string> ALLOWED_MULTIHEADERS;

		enum ParsingState { UNDEFINED, HEADER, USERHEADERS, BODY, FINISHED };

	private:
		HeaderMap	 headers;
		Url			 url;
		Method		 method;
		Option<Body> body;
		ParsingState state;
		std::string	 tmp_buffer;

	public:
		const HeaderMap&	getHeaders() const { return this->headers; };
		const Url&			getUrl() const { return this->url; };
		const Method&		getMethod() const { return this->method; };
		const Option<Body>& getBody() const { return this->body; };

		HeaderMap&	  getHeaders() { return this->headers; };
		Url&		  getUrl() { return this->url; };
		Method&		  getMethod() { return this->method; };
		Option<Body>& getBody() { return this->body; };
		ParsingState  getState() { return this->state; };

		void parseBytes(std::vector<char>::const_iterator start,
						std::vector<char>::const_iterator end);

		Request() : state(HEADER) {};
		~Request() {};
		Request(const Request& rhs) {
			this->headers	 = rhs.headers;
			this->url		 = rhs.url;
			this->method	 = rhs.method;
			this->body		 = rhs.body;
			this->state		 = rhs.state;
			this->tmp_buffer = rhs.tmp_buffer;
		};

		Request& operator=(const Request& rhs) {
			if (this != &rhs) {
				this->headers	 = rhs.headers;
				this->url		 = rhs.url;
				this->method	 = rhs.method;
				this->body		 = rhs.body;
				this->state		 = rhs.state;
				this->tmp_buffer = rhs.tmp_buffer;
			}
			return (*this);
		};
};

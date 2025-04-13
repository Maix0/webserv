/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:51:48 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/09 17:12:16 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <sstream>
#include <string>
#include <vector>
#include "app/http/StatusCode.hpp"
#include "config/Config.hpp"
#include "lib/IndexMap.hpp"
#include "runtime/Logger.hpp"

#define CRLF "\r\n"

class Request {
	public:
		typedef IndexMap<std::string, std::string> HeaderMap;
		typedef std::string						   Url;
		typedef std::string						   Method;

		static const std::vector<std::string> ALLOWED_MULTIHEADERS;
		static const std::size_t			  MAX_URI_SIZE	   = 1 << 10;
		static const std::size_t			  MAX_HEADERS_SIZE = 1 << 16;

		enum ParsingState { UNDEFINED, HEADER, USERHEADERS, BODY, FINISHED };

	private:
		HeaderMap	 headers;
		Url			 url;
		Method		 method;
		ParsingState state;
		size_t		 headers_total_size;

		// POST DATA FOR BODIES
		int		body_fd;
		size_t	body_size;
		ssize_t content_length;

		// Weak reference to the server configs
		const config::Server* server;
		const config::Route*  route;
		Port				  port;

	public:
		ParsingState		  getState() const { return this->state; };
		const HeaderMap&	  getHeaders() const { return this->headers; };
		const Method&		  getMethod() const { return this->method; };
		const Url&			  getUrl() const { return this->url; };
		const config::Route*  getRoute() const { return this->route; };
		const config::Server* getServer() const { return this->server; };

		HeaderMap&	 getHeaders() { return this->headers; };
		Method&		 getMethod() { return this->method; };
		ParsingState getState() { return this->state; };
		Url&		 getUrl() { return this->url; };

		bool parseBytes(std::string& buffer);

		Request(Port port, const config::Server* default_server)
			: state(HEADER),
			  headers_total_size(0),
			  body_fd(-1),
			  body_size(0),
			  content_length(-1),
			  server(default_server),
			  route(NULL),
			  port(port) {};
		~Request() {
			if (this->body_fd != -1)
				close(this->body_fd);
		};
		void setFinished() { this->state = FINISHED; };

		int copyBodyFd() {
			if (this->body_fd == -1)
				return -1;
			int out = -1;
			_ERR_RET_THROW(out = dup(this->body_fd));
			return out;
		}

		class PageException : public std::exception {
			private:
				std::string str;
				StatusCode	code;

			public:
				virtual ~PageException() throw() {}
				PageException(StatusCode code) : code(code) {
					std::stringstream ss;
					ss << "requested Page for status code " << code.code();
					this->str = ss.str();
				}
				virtual const char* what(void) const throw() { return this->str.c_str(); }
				StatusCode			statusCode() const { return this->code; };
		};
};

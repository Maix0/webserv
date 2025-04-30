/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:51:48 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/28 20:30:51 by maiboyer         ###   ########.fr       */
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
#include "lib/Option.hpp"
#include "lib/Rc.hpp"
#include "lib/TempFile.hpp"
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

		enum ParsingState {
			UNDEFINED,
			HEADER,
			USERHEADERS,
			PREBODY,
			BODY,

			CHUNKED_BODY_HEADER,
			CHUNKED_BODY_NEW_CHUNK,
			CHUNKED_BODY_READ_CHUNK,
			CHUNKED_BODY_END_CHUNK,
			CHUNKED_BODY_DONE,

			FINISHED

		};

		const static std::map<Request::ParsingState, std::string> STATE_TO_STR;
		static std::string										  state_to_str(ParsingState state) {
			   if (STATE_TO_STR.count(state))
				   return STATE_TO_STR.at(state);
			   else
				   return "Unknown";
		}

	private:
		HeaderMap	 headers;
		Url			 url;
		Method		 method;
		ParsingState state;
		size_t		 headers_total_size;

		Ip ip;

		// POST DATA FOR BODIES
		Option<Rc<tiostream> > body;
		size_t				   body_size;
		ssize_t				   content_length;

		// Weak reference to the server configs
		const config::Server* server;
		const config::Route*  route;
		Port				  port;

		size_t current_chunk_size;
		size_t remaining_chunk_size;

	public:
		Option<Rc<tiostream> > getBody() const { return this->body; };
		ParsingState		   getState() const { return this->state; };
		const HeaderMap&	   getHeaders() const { return this->headers; };
		const Method&		   getMethod() const { return this->method; };
		const Url&			   getUrl() const { return this->url; };
		const config::Route*   getRoute() const { return this->route; };
		const config::Server*  getServer() const { return this->server; };

		HeaderMap&	 getHeaders() { return this->headers; };
		Method&		 getMethod() { return this->method; };
		ParsingState getState() { return this->state; };
		Url&		 getUrl() { return this->url; };
		std::size_t	 getBodySize() { return this->body_size; };

		Port getPort() { return this->port; };
		Ip	 getIp() { return this->ip; };

		bool parseBytes(std::string& buffer);

		Request(Ip ip, Port port, const config::Server* default_server)
			: state(HEADER),
			  headers_total_size(0),
			  ip(ip),
			  body(),
			  body_size(0),
			  content_length(-1),
			  server(default_server),
			  route(NULL),
			  port(port) {};
		~Request() {};
		void setFinished() { this->state = FINISHED; };

		class PageException : public std::exception {
			private:
				std::string str;
				StatusCode	code;
				bool		with_body;

			public:
				virtual ~PageException() throw() {}
				/*
				PageException(StatusCode code) : code(code), with_body(true) {
					std::stringstream ss;
					ss << "requested Page for status code " << code.code();
					this->str = ss.str();
					LOG(warn, this->str);
				}
				*/
				PageException(StatusCode code, bool with_body) : code(code), with_body(with_body) {
					std::stringstream ss;
					ss << "requested Page for status code " << code.code();
					this->str = ss.str();
					LOG(warn, this->str);
				}
				virtual const char* what(void) const throw() { return this->str.c_str(); }
				StatusCode			statusCode() const { return this->code; };
				bool				withBody() const { return this->with_body; };
		};
};

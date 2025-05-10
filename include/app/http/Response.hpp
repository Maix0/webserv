/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 15:39:45 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/10 12:21:19 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <deque>
#include <istream>
#include <sstream>
#include <string>

#include "app/http/CgiOutput.hpp"
#include "app/http/StatusCode.hpp"
#include "config/Config.hpp"
#include "lib/Rc.hpp"
#include "runtime/Epoll.hpp"

#ifndef SERVER_NAME
#	define SERVER_NAME "Maixserv"
#endif

#define COPY_BUFFER_SIZE (1 << 22)

class Connection;

class Response {
	public:
		typedef IndexMap<std::string, std::string> HeaderMap;

	private:
		HeaderMap  headers;
		StatusCode code;

		// POST DATA FOR BODIES
		std::size_t		 body_size;
		Rc<std::istream> body;
		std::deque<char> inner_buffer;

		bool sent_headers;
		bool is_finished;
		bool is_stream_eof;

		Option<Rc<CgiOutput> > passthru;
		std::string			   method;

	public:
		void setMethod(std::string method) { this->method = method; };
		Response()
			: code(StatusCode(200)),
			  body_size(0),
			  body(Rc<std::stringstream>().cast<std::istream>()),
			  sent_headers(false),
			  is_finished(false),
			  is_stream_eof(false) {};
		~Response();
		void setCgi(Rc<CgiOutput>& s) { this->passthru = s; };

		void setBody(Rc<std::istream> body, size_t size) {
			this->body		= body;
			this->body_size = size;
		};

		std::size_t getBodySize() { return this->body_size; };

		void			 setFinished() { this->is_finished = true; };
		bool			 isFinished() { return this->is_finished; };
		bool			 isEof() { return this->is_stream_eof; };
		Rc<std::istream> getBody() { return this->body; };

		void	   setStatus(StatusCode code) { this->code = code; };
		StatusCode getStatus() { return this->code; };

		Option<std::string> getHeader(const std::string& name);
		bool				setHeader(std::pair<std::string, std::string> pair);
		bool				setHeader(std::string name, std::string value) {
			   return this->setHeader(std::make_pair(name, value));
		}

		void setMimeType(const std::string& extension);
		void setMimeTypeRaw(const std::string& mime_type);

		static Rc<Response> createStatusPageFor(Epoll&				  epoll,
												Rc<Connection>&		  conn,
												const config::Server* server,
												StatusCode			  code,
												bool				  with_body = true);
		static Rc<Response> createResponseFor(Epoll& epoll, Rc<Connection>& conn);

		std::size_t fill_buffer(char buf[], std::size_t len);
		void		sent_bytes(std::size_t len);
};

namespace handlers {

	void handle_static_file(Epoll&			epoll,
							Rc<Connection>& connection,
							Rc<Request>&	req,
							Rc<Response>&	res);
	void handle_post_delete(Epoll&			epoll,
							Rc<Connection>& connection,
							Rc<Request>&	req,
							Rc<Response>&	res);
	void handle_redirect(Epoll&			 epoll,
						 Rc<Connection>& connection,
						 Rc<Request>&	 req,
						 Rc<Response>&	 res);

	void		 handle_cgi_request(Epoll&			   epoll,
									Rc<Connection>&	   connection,
									Rc<Request>&	   req,
									Rc<Response>&	   res,
									const config::Cgi& cgi,
									std::string		   cgi_prefix);
	Rc<Response> default_status_page(StatusCode code, bool with_body);

	const config::Cgi* find_cgi_for(const Url&			 url,
									const config::Route& route,
									std::string&		 cgi_suffix);
}	// namespace handlers

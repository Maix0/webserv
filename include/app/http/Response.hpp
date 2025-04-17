/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 15:39:45 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/17 18:05:56 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <deque>
#include <istream>
#include <sstream>
#include <string>

#include "app/http/StatusCode.hpp"
#include "config/Config.hpp"
#include "lib/Rc.hpp"
#include "runtime/Epoll.hpp"

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

	public:
		Response()
			: code(StatusCode(200)),
			  body(Rc<std::istream>(new std::stringstream())),
			  sent_headers(false),
			  is_finished(false) {};
		~Response() {};

		void setBody(Rc<std::istream> body, size_t size) {
			this->body		= body;
			this->body_size = size;
		};
		void			 setFinished() { this->is_finished = true; };
		bool			 isFinished() { return this->is_finished; };
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
												Rc<Connection>		  conn,
												const config::Server* server,
												StatusCode			  code);
		static Rc<Response> createResponseFor(Epoll& epoll, Rc<Connection> conn);

		std::size_t fill_buffer(char buf[], std::size_t len);
		void		sent_bytes(std::size_t len);
};

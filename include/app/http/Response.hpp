/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 15:39:45 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/14 00:12:58 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <istream>
#include <sstream>
#include <string>
#include "app/http/StatusCode.hpp"
#include "app/net/Connection.hpp"
#include "config/Config.hpp"

class Response {
	public:
		typedef IndexMap<std::string, std::string> HeaderMap;

	private:
		HeaderMap  headers;
		StatusCode code;

		// POST DATA FOR BODIES
		Rc<std::istream>  body;
		std::vector<char> inner_buffer;

	public:
		Response()
			: code(StatusCode(200)),
			  body(Rc<std::stringstream>(new std::stringstream()).cast<std::istream>()) {};
		~Response() {};

		void			 setBody(Rc<std::istream> body) { this->body = body; };
		Rc<std::istream> getBodyFd() { return this->body; };

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

		std::string toBytes();
};

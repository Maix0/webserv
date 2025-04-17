/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:48:32 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/17 18:18:34 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <deque>
#include <ios>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "app/http/MimesTypes.hpp"
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "app/net/Connection.hpp"
#include "runtime/Logger.hpp"

bool Response::setHeader(std::pair<std::string, std::string> pair) {
	return this->headers.insert(pair).second;
}

Option<std::string> Response::getHeader(const std::string& name) {
	try {
		return Option<std::string>::Some(this->headers.at(name));
	} catch (const std::out_of_range&) {
		return Option<std::string>::None();
	}
}

void Response::setMimeType(const std::string& extension) {
	this->setHeader("Content-Type", mime::MimeType::from_extension(extension).getInner());
}

void Response::setMimeTypeRaw(const std::string& raw) {
	this->setHeader("Content-Type", raw);
}

static Rc<Response> default_status_page(StatusCode code) {
	Rc<Response>		  res		= new Response();
	const std::string	  canonical = code.canonical().get_or("Unknown code");
	Rc<std::stringstream> body;

	(*body) << "<html>" CRLF;
	(*body) << "<head><title> " << code.code() << " - " << canonical << " "
			<< "</title></head>" CRLF;
	(*body) << "<body>" CRLF;
	(*body) << "<center><h1>" << code.code() << " - " << canonical << " "
			<< "</h1></center>" CRLF;

	res->setStatus(code);
	res->setBody(body.cast<std::istream>(), body->str().size());
	res->setMimeType("txt");
	return res;
}

Rc<Response> Response::createStatusPageFor(Epoll&				 epoll,
										   Rc<Connection>		 conn,
										   const config::Server* server,
										   StatusCode			 code) {
	(void)(epoll);
	(void)(conn);
	std::stringstream s;
	s << code.code();
	std::string c = s.str();
	if (server == NULL || server->errors.count(c) == 0)
		return (default_status_page(code));

	return (default_status_page(code));
}

Rc<Response> Response::createResponseFor(Epoll& epoll, Rc<Connection> connection) {
	std::string res;

	Request& req = connection->getRequest();
	if (req.getMethod() == "HEAD") {
	} else if (req.getMethod() == "GET") {
	} else if (req.getMethod() == "POST")
		connection->getOutBuffer().insert(connection->getOutBuffer().end(), res.begin(), res.end());
	connection->getRequest().setFinished();
	connection->getRequest() =
		Request(connection->getSocket()->getPort(), connection->getSocket()->getServer());
	connection->getResponse() = default_status_page(status::OK);
	LOG(info, "connection.body_size == " << connection->getResponse()->body_size);
	{
		Rc<ConnectionCallback<WRITE> > con = new ConnectionCallback<WRITE>(connection);
		epoll.addCallback(connection->asFd(), WRITE, con.cast<Callback>());
	}

	return (connection->getResponse());
}

#define READ_BUF 2048

std::size_t Response::fill_buffer(char buf[], std::size_t len) {
	if (!this->sent_headers) {
		std::stringstream ss;
		ss << "HTTP/1.1 " << this->code.code() << " " << this->code.canonical() << CRLF;
		for (HeaderMap::iterator it = this->headers.begin(); it != this->headers.end(); it++)
			ss << it->first << ": " << it->second << CRLF;
		ss << "content-length: " << this->body_size;
		ss << CRLF	CRLF;
		std::string out = ss.str();
		this->inner_buffer.insert(this->inner_buffer.end(), out.begin(), out.end());
		this->sent_headers = true;
	}
	if (this->inner_buffer.size() <= len) {
		char buffer[READ_BUF] = {};
		this->body->read(buffer, READ_BUF);
		this->inner_buffer.insert(this->inner_buffer.end(), &buffer[0],
								  &buffer[this->body->gcount()]);
	}
	std::size_t i = 0;
	for (std::deque<char>::iterator it = this->inner_buffer.begin();
		 it != this->inner_buffer.end() && i < len; it++, i++)
		buf[i] = *it;
	LOG(info, "filled buffer with " << i << "bytes when asked for " << len << "max");
	this->inner_buffer.erase(this->inner_buffer.begin(), this->inner_buffer.begin() + i);
	return i;
}

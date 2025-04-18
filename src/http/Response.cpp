/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:48:32 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/18 14:40:04 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <ctime>
#include <deque>
#include <exception>
#include <ios>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "app/fs/ServerRessources.hpp"
#include "app/http/MimesTypes.hpp"
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "app/http/StatusCode.hpp"
#include "app/net/Connection.hpp"
#include "runtime/Logger.hpp"

void add_common_header(Response& res) {
	char	   buffer[1024] = {};
	struct tm* tm_info;
	time_t	   tim = time(NULL);

	// Convert seconds part to struct tm (local time)
	tm_info		   = localtime(&tim);

	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", tm_info);
	res.setHeader("Date", buffer);
	res.setHeader("Server", SERVER_NAME);
}

static Rc<Response> default_status_page(StatusCode code) {
	Rc<Response>		  res		= new Response();
	const std::string	  canonical = code.canonical().get_or("Unknown code");
	Rc<std::stringstream> body;

	(*body) << "<html>" CRLF;
	(*body) << "<head><title> " << code.code() << " - " << canonical << "</title></head>" CRLF;
	(*body) << "<body>" CRLF;
	(*body) << "<center><h1>" << code.code() << " - " << canonical << "</h1></center>" CRLF;
	(*body) << "<center><small>" << "Server: " << SERVER_NAME << "</small></center>" CRLF;
	(*body) << "</body>" CRLF;
	(*body) << "</html>" CRLF;

	res->setStatus(code);
	res->setBody(body.cast<std::istream>(), body->str().size());
	res->setMimeType("html");
	return res;
}

void handle_head_get(Epoll& epoll, Rc<Connection> connection, Rc<Request> req, Rc<Response> res) {
	(void)(epoll);
	try {
		std::size_t		 body_size;
		std::string		 ext;
		Rc<std::istream> body =
			getFileAt(req->getUrl(), req->getServer(), req->getRoute(), &ext, &body_size);
		res->setBody(body, body_size);
		res->setMimeType(ext);
		res->setStatus(200);
	} catch (const fs::error::NotAllowed& e) {
		connection->getResponse() =
			Response::createStatusPageFor(epoll, connection, req->getServer(), status::FORBIDDEN);
	} catch (const fs::error::NotFound& e) {
		connection->getResponse() =
			Response::createStatusPageFor(epoll, connection, req->getServer(), status::NOT_FOUND);
	} catch (const fs::error::IsADirectory& e) {
		connection->getResponse() =
			Response::createStatusPageFor(epoll, connection, req->getServer(), status::NOT_FOUND);
	} catch (const std::exception& e) {
		LOG(warn, "got error: " << e.what());
		connection->getResponse() = Response::createStatusPageFor(
			epoll, connection, req->getServer(), status::INTERNAL_SERVER_ERROR);
	} catch (...) {
		LOG(warn, "got unknown error: ");
		connection->getResponse() = Response::createStatusPageFor(
			epoll, connection, req->getServer(), status::INTERNAL_SERVER_ERROR);
	}

	if (req->getMethod() == "HEAD")
		connection->getResponse()->setBody(new std::stringstream(),
										   connection->getResponse()->getBodySize());
}

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
	Rc<Request> req = connection->getRequest();
	req->setFinished();
	connection->getRequest() =
		new Request(connection->getSocket()->getPort(), connection->getSocket()->getServer());
	Rc<Response> res = (connection->getResponse() = new Response());

	LOG(info, "req->method == " << req->getMethod());
	if (req->getMethod() == "HEAD" || req->getMethod() == "GET") {
		handle_head_get(epoll, connection, req, res);
	} else if (req->getMethod() == "POST" || req->getMethod() == "PUT")
		;
	{
		Rc<ConnectionCallback<WRITE> > con = new ConnectionCallback<WRITE>(connection);
		epoll.addCallback(connection->asFd(), WRITE, con.cast<Callback>());
	}
	add_common_header(*connection->getResponse());
	return (connection->getResponse());
}

#define READ_BUF 2048

std::size_t Response::fill_buffer(char buf[], std::size_t len) {
	if (!this->sent_headers) {
		std::stringstream ss;
		ss << "HTTP/1.1 " << this->code.code() << " "
		   << this->code.canonical().get_or("Unknown Code") << CRLF;
		for (HeaderMap::iterator it = this->headers.begin(); it != this->headers.end(); it++)
			ss << it->first << ": " << it->second << CRLF;
		ss << "Content-Length: " << this->body_size;
		ss << CRLF	CRLF;
		std::string out = ss.str();
		this->inner_buffer.insert(this->inner_buffer.end(), out.begin(), out.end());
		this->sent_headers = true;
	}
	while (this->inner_buffer.size() <= len) {
		char buffer[READ_BUF] = {};
		this->body->read(buffer, READ_BUF);
		size_t l = READ_BUF;
		if (this->body->eof() || this->body->fail())
			l = this->body->gcount();
		this->inner_buffer.insert(this->inner_buffer.end(), &buffer[0], &buffer[l]);
		if (l < READ_BUF)
			break;
	}
	std::size_t i = 0;
	for (std::deque<char>::iterator it = this->inner_buffer.begin();
		 it != this->inner_buffer.end() && i < len; it++, i++)
		buf[i] = *it;
	this->inner_buffer.erase(this->inner_buffer.begin(), this->inner_buffer.begin() + i);
	return i;
}

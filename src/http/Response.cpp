/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:48:32 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/14 00:48:09 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
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
	res->setBody(body.cast<std::istream>());
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
	{
		Rc<ConnectionCallback<WRITE> > con = new ConnectionCallback<WRITE>(connection);
		epoll.addCallback(connection->asFd(), WRITE, con.cast<Callback>());
	}
	connection->getRequest().setFinished();
	connection->getRequest() =
		Request(connection->getSocket()->getPort(), connection->getSocket()->getServer());
	_UNREACHABLE;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:48:32 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/18 17:06:03 by maiboyer         ###   ########.fr       */
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

#include "app/State.hpp"
#include "app/fs/ServerRessources.hpp"
#include "app/http/MimesTypes.hpp"
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "app/http/StatusCode.hpp"
#include "app/net/Connection.hpp"
#include "config/Config.hpp"
#include "lib/StringHelper.hpp"
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

void handle_redirect(Epoll& epoll, Rc<Connection> connection, Rc<Request> req, Rc<Response> res) {
	(void)(epoll);
	(void)(connection);

	static const StatusCode CODE	  = status::MOVED_PERMANENTLY;
	const std::string		canonical = CODE.canonical().get_or("Unknown code");
	Rc<std::stringstream>	body;

	LOG(info, "handle request !");
	(*body) << "<html>" CRLF;
	(*body) << "<head><title> Redirection - " << string_escape_html(req->getRoute()->redirect.get())
			<< "</title></head>" CRLF;
	(*body) << "<body>" CRLF;
	(*body) << "<center><h1>" << "you are being redirected to "
			<< string_escape_html(req->getRoute()->redirect.get()) << "</h1></center>" CRLF;
	(*body) << "<center><small>" << "Server: " << SERVER_NAME << "</small></center>" CRLF;
	(*body) << "</body>" CRLF;
	(*body) << "</html>" CRLF;

	res->setStatus(CODE);
	res->setBody(body.cast<std::istream>(), body->str().size());
	res->setMimeType("html");
	res->setHeader("Location", req->getRoute()->redirect.get());
}

void handle_static_file(Epoll&		   epoll,
						Rc<Connection> connection,
						Rc<Request>	   req,
						Rc<Response>   res) {
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

void handle_cgi_request(Epoll&			   epoll,
						Rc<Connection>	   connection,
						Rc<Request>		   req,
						Rc<Response>	   res,
						const config::Cgi& cgi) {
	(void)(epoll);
	(void)(connection);
	(void)(req);
	(void)(res);
	(void)(cgi);
	throw Request::PageException(status::NOT_IMPLEMENTED);
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
	Rc<Response>						res	 = (connection->getResponse() = new Response());
	std::map<std::string, config::Cgi>& cgis = State::getInstance().getConfig().cgi;

	if (req->getRoute() == NULL) {
		handle_static_file(epoll, connection, req, res);
		LOG(info, "handle static file");
	} else {
		LOG(debug, *req->getRoute());
		if (req->getRoute()->redirect.hasValue()) {
			handle_redirect(epoll, connection, req, res);
			LOG(info, "handle redirect");
		} else {
			const config::Cgi* cgi = NULL;
			(void)(cgi);
			std::string::size_type slash_pos = req->getUrl().find('/');
			std::string			   last_part;
			{
				std::string::iterator start = req->getUrl().begin();
				if (slash_pos != std::string::npos)
					start += slash_pos + 1;
				last_part = std::string(start, req->getUrl().end());
			}
			for (std::map<std::string, std::string>::const_iterator it =
					 req->getRoute()->cgi.begin();
				 it != req->getRoute()->cgi.end(); it++) {
				if (string_ends_with(last_part, it->second)) {
					cgi = &cgis[it->second];
					break;
				}
			}
			if (cgi) {
				handle_cgi_request(epoll, connection, req, res, *cgi);
				LOG(info, "handle cgi");
			} else {
				LOG(info, "handle fallback");
			}
		}
	}
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

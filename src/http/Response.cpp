/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:48:32 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/10 12:21:38 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <ctime>
#include <deque>
#include <exception>
#include <fstream>
#include <ios>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "app/State.hpp"
#include "app/fs/ServerRessources.hpp"
#include "app/http/CgiOutput.hpp"
#include "app/http/MimesTypes.hpp"
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "app/http/StatusCode.hpp"
#include "app/http/Url.hpp"
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

Rc<Response> Response::createResponseFor(Epoll& epoll, Rc<Connection>& connection) {
	Rc<Request> req = connection->getRequest();
	req->setFinished();
	connection->getRequest() =
		Rc<Request>(Functor3<Request, Ip, Port, config::Server*>(
						connection->getIp(), connection->getSocket()->getPort(),
						connection->getSocket()->getServer()),
					RCFUNCTOR);
	Rc<Response>& res = connection->getResponse() = Rc<Response>();

	std::string method							  = req->getMethod();
	res->method									  = method;
	if (req->getRoute() && req->getRoute()->allowed.hasValue() &&
		std::find(req->getRoute()->allowed.get().begin(), req->getRoute()->allowed.get().end(),
				  method) == req->getRoute()->allowed.get().end()) {
		LOG(debug, "not allowed in route...");
		throw Request::PageException(status::METHOD_NOT_ALLOWED, req->getMethod() != "HEAD");
	}

	LOG(info, "making response for" << req->getUrl().getAll());
	// the poor man block-breaking capability...
	do {
		// we have no route, default to server-level handling
		if (req->getRoute() == NULL) {
			handlers::handle_static_file(epoll, connection, req, res);
			break;
		}
		// the route we matched is a redirect
		if (req->getRoute()->redirect.hasValue()) {
			handlers::handle_redirect(epoll, connection, req, res);
			break;
		}
		std::string		   cgi_prefix;
		const config::Cgi* cgi =
			handlers::find_cgi_for(req->getUrl(), *req->getRoute(), cgi_prefix);
		// the route we matched is cgi
		if (cgi) {
			handlers::handle_cgi_request(epoll, connection, req, res, *cgi, cgi_prefix);
			break;
		}
		// no special threatment to be done, do whatever :)
		handlers::handle_static_file(epoll, connection, req, res);
	} while (0);
	{
		Rc<ConnectionCallback<WRITE> > cbw = Rc<ConnectionCallback<WRITE> >(
			Functor1<ConnectionCallback<WRITE>, Rc<Connection>&>(connection), RCFUNCTOR);
		epoll.addCallback(connection->asFd(), WRITE, cbw.cast<Callback>());
	}
	add_common_header(*connection->getResponse());
	// we strip the body if req->method == HEAD :)
	if (req->getMethod() == "HEAD") {
		LOG(info, "HEAD request, should remove body....");
		connection->getResponse()->setBody(Rc<std::stringstream>().cast<std::istream>(),
										   connection->getResponse()->getBodySize());
	}
	return (connection->getResponse());
}

#define READ_BUF 2048

std::size_t Response::fill_buffer(char buf[], std::size_t len) {
	// we have to wait until the cgi is done...
	if (this->passthru.hasValue() && !this->passthru.get()->isFinished())
		return 0;

	if (!this->sent_headers) {
		if (this->method == "HEAD") {
			this->body = Rc<std::stringstream>().cast<std::istream>();
		}
		std::stringstream ss;
		ss << "HTTP/1.1 " << this->code.code() << " "
		   << this->code.canonical().get_or("Unknown Code") << CRLF;
		for (HeaderMap::iterator it = this->headers.begin(); it != this->headers.end(); it++)
			ss << it->first << ": " << it->second << CRLF;
		ss << "Content-Length: " << this->body_size;
		ss << CRLF	CRLF;
		std::string out = ss.str();
		LOG(debug, "Content-Length: " << this->body_size);
		this->inner_buffer.insert(this->inner_buffer.end(), out.begin(), out.end());
		this->sent_headers = true;
	}
	while (this->inner_buffer.size() <= len) {
		char buffer[READ_BUF] = {};
		this->body->read(buffer, READ_BUF);
		size_t l = this->body->gcount();
		if (this->body->eof() || this->body->fail()) {
			this->is_stream_eof = true;
		}
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

Response::~Response() {
	if (this->passthru.hasValue()) {
		CgiOutput* cptr	 = &*this->passthru.get();
		CgiList&   clist = State::getInstance().getCgis();
		bool	   found;
		do {
			found = false;
			for (CgiList::iterator it = clist.begin(); it != clist.end(); it++) {
				if (&**it == cptr) {
					clist.erase(it);
					found = true;
					break;
				}
			}
		} while (found);
	}
};

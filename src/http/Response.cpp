/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:48:32 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/26 23:38:27 by maiboyer         ###   ########.fr       */
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
#include "app/http/Routing.hpp"
#include "app/http/StatusCode.hpp"
#include "app/net/Connection.hpp"
#include "config/Config.hpp"
#include "lib/StringHelper.hpp"
#include "runtime/Logger.hpp"

#define COPY_BUFFER_SIZE (1 << 22)

static Rc<Response> default_status_page(StatusCode code, bool with_body) {
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
	res->setBody(with_body ? body.cast<std::istream>() : new std::stringstream, body->str().size());
	res->setMimeType("html");
	return res;
}

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

void handle_post_delete(Epoll&		   epoll,
						Rc<Connection> connection,
						Rc<Request>	   req,
						Rc<Response>   res) {
	(void)(epoll);
	(void)(connection);
	/// if no route, we CAN'T process those request since we don't have a post/upload dir
	if (!req->getRoute()) {
		LOG(debug, "no route...");
		throw Request::PageException(status::METHOD_NOT_ALLOWED, req->getMethod() != "HEAD");
	}
	const config::Route& route = *req->getRoute();
	/// Well we have a route, but it doesn't have a post_dir. this shoul de caught before, but we
	/// don't know yet;
	if (!route.post_dir.hasValue()) {
		LOG(debug, "no post dir");
		throw Request::PageException(status::METHOD_NOT_ALLOWED, req->getMethod() != "HEAD");
	}
	std::string file				   = route.post_dir.get();
	file							  += "/";
	std::vector<std::string> url_parts = url_to_parts(req->getUrl());
	url_parts.erase(url_parts.begin(),
					url_parts.begin() + (std::min(url_parts.size(), route.parts.size())));
	if (url_parts.empty()) {
		if (route.index.hasValue())
			url_parts.push_back(route.index.get());
		else
			throw Request::PageException(status::NOT_FOUND, req->getMethod() != "HEAD");
	}

	for (std::vector<std::string>::iterator it = url_parts.begin(); it != url_parts.end(); it++) {
		// no directory traversal for you :)
		if (*it == ".." || *it == ".")
			throw Request::PageException(status::SERVICE_UNAVAILABLE, req->getMethod() != "HEAD");
		file += "/";
		file += *it;
	}
	// remove all `//` and replace them with a single /
	for (std::string::size_type pos = file.find("//"); pos != std::string::npos;
		 pos						= file.find("//"))
		   file.replace(pos, 2, "/");

	struct stat s;
	int			sres = stat(file.c_str(), &s);
	int			serr = errno;
	if (req->getMethod() == "POST") {
		std::ofstream file_out;
		file_out.open(file.c_str(), std::ios_base::out | std::ios_base::binary);
		if (file_out.fail())
			throw Request::PageException(status::INTERNAL_SERVER_ERROR, req->getMethod() != "HEAD");
		char*		  buffer = new char[COPY_BUFFER_SIZE];
		Rc<tiostream> body	 = req->getBody().get();
		body->seekg(0, std::ios_base::beg);
		if (body->fail())
			LOG(err, "failed to seekg");
		body->seekp(0, std::ios_base::beg);
		if (body->fail())
			LOG(err, "failed to seekp");
		while (!(body->eof() || body->fail() || file_out.fail())) {
			body->read(buffer, COPY_BUFFER_SIZE);
			size_t len = body->gcount();
			file_out.write(buffer, len);
		}
		delete[] buffer;
		if ((body->bad() || file_out.bad()))
			throw Request::PageException(status::INTERNAL_SERVER_ERROR, req->getMethod() != "HEAD");
		res->setStatus(status::NO_CONTENT);
	} else if (req->getMethod() == "DELETE") {
		// no file
		if (sres == -1 && serr == ENOENT)
			throw Request::PageException(status::NOT_FOUND, req->getMethod() != "HEAD");
		// can't write to it == can't delete it
		if (!(s.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)))
			throw Request::PageException(status::FORBIDDEN, req->getMethod() != "HEAD");
		// trying to remove the file/directory :)
		if (unlink(file.c_str())) {
			int serr2 = errno;
			(void)(serr2);
			LOG(warn, "failed to unlink file '" << file << "': " << strerror(serr2));
			throw Request::PageException(status::INTERNAL_SERVER_ERROR, req->getMethod() != "HEAD");
		}
		res->setStatus(status::NO_CONTENT);
	}
}

void handle_static_file(Epoll&		   epoll,
						Rc<Connection> connection,
						Rc<Request>	   req,
						Rc<Response>   res) {
	(void)(epoll);
	if (req->getMethod() == "POST" || req->getMethod() == "DELETE") {
		handle_post_delete(epoll, connection, req, res);
		return;
	}
	try {
		std::size_t body_size = 0;
		std::string ext;
		std::string file_path;
		{
			std::vector<std::string>		url_parts	= url_to_parts(req->getUrl());
			const std::vector<std::string>* route_parts = NULL;
			if (req->getRoute())
				route_parts = &req->getRoute()->parts;

			url_parts.erase(url_parts.begin(),
							url_parts.begin() + (std::min(url_parts.size(),
														  route_parts ? route_parts->size() : 0)));

			for (std::vector<std::string>::iterator it = url_parts.begin(); it != url_parts.end();
				 it++) {
				// no directory traversal for you :)
				if (it->empty())
					continue;
				if (*it == ".." || *it == ".")
					throw Request::PageException(status::SERVICE_UNAVAILABLE,
												 req->getMethod() != "HEAD");
				file_path += "/";
				file_path += *it;
			}
			// remove all `//` and replace them with a single /
			for (std::string::size_type pos = file_path.find("//"); pos != std::string::npos;
				 pos						= file_path.find("//"))
				   file_path.replace(pos, 2, "/");
		}
		std::size_t* bptr = &body_size;
		assert(bptr != NULL);
		*bptr = 1;
		assert(body_size == 1);
		body_size = 0;
		assert(*bptr == 0);
		Rc<std::istream> body =
			getFileAt(file_path, req->getServer(), req->getRoute(), &ext, bptr, NULL);
		res->setBody(body, body_size);
		res->setMimeType(ext);
		res->setStatus(200);
		connection->getResponse() = res;
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
		connection->getResponse() =
			Response::createStatusPageFor(epoll, connection, req->getServer(), status::NOT_FOUND);
	}
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

	CgiList& cgi_list = State::getInstance().getCgis();
	assert(!cgi.binary.empty());
	Rc<CgiOutput> o = new CgiOutput(epoll, req, cgi.binary, res, *connection);
	cgi_list.push_back(o);
	res->setCgi(o);
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
										   StatusCode			 code,
										   bool					 with_body) {
	(void)(epoll);
	(void)(conn);
	std::stringstream s;
	s << code.code();
	std::string c = s.str();
	if (server == NULL || server->errors.count(c) == 0)
		return (default_status_page(code, with_body));
	return (default_status_page(code, with_body));
}

const config::Cgi* find_cgi_for(const std::string& url, const config::Route& route) {
	std::map<std::string, config::Cgi>& all_cgis  = State::getInstance().getConfig().cgi;
	const config::Cgi*					cgi		  = NULL;
	std::string::size_type				slash_pos = url.find('/');
	std::string::size_type				qs_pos	  = url.find('?');
	std::string							last_part;

	{
		std::string::const_iterator start = url.begin();
		std::string::const_iterator end	  = url.end();
		if (qs_pos != std::string::npos)
			end = url.begin() + qs_pos;
		if (slash_pos != std::string::npos)
			start += slash_pos + 1;
		last_part = std::string(start, url.end());
	}

	for (std::map<std::string, std::string>::const_iterator it = route.cgi.begin();
		 it != route.cgi.end(); it++) {
		if (!last_part.empty() && !it->first.empty() && string_ends_with(last_part, it->first)) {
			cgi = &all_cgis.at(it->second);
			break;
		}
	}
	return cgi;
}

Rc<Response> Response::createResponseFor(Epoll& epoll, Rc<Connection> connection) {
	Rc<Request> req = connection->getRequest();
	req->setFinished();
	connection->getRequest() = new Request(connection->getIp(), connection->getSocket()->getPort(),
										   connection->getSocket()->getServer());
	Rc<Response> res		 = (connection->getResponse() = new Response());
	std::string	 method		 = req->getMethod();
	res->method				 = method;
	if (req->getRoute() && req->getRoute()->allowed.hasValue() &&
		std::find(req->getRoute()->allowed.get().begin(), req->getRoute()->allowed.get().end(),
				  method) == req->getRoute()->allowed.get().end()) {
		LOG(debug, "not allowed in route...");
		throw Request::PageException(status::METHOD_NOT_ALLOWED, req->getMethod() != "HEAD");
	}

	// the poor man block-breaking capability...
	do {
		// we have no route, default to server-level handling
		if (req->getRoute() == NULL) {
			handle_static_file(epoll, connection, req, res);
			break;
		}
		// the route we matched is a redirect
		if (req->getRoute()->redirect.hasValue()) {
			handle_redirect(epoll, connection, req, res);
			break;
		}
		const config::Cgi* cgi = find_cgi_for(req->getUrl(), *req->getRoute());
		// the route we matched is cgi
		if (cgi) {
			handle_cgi_request(epoll, connection, req, res, *cgi);
			break;
		}
		// no special threatment to be done, do whatever :)
		handle_static_file(epoll, connection, req, res);
	} while (0);
	epoll.addCallback(connection->asFd(), WRITE, new ConnectionCallback<WRITE>(connection));
	add_common_header(*connection->getResponse());
	// we strip the body if req->method == HEAD :)
	if (req->getMethod() == "HEAD") {
		LOG(info, "HEAD request, should remove body....");
		connection->getResponse()->setBody(new std::stringstream(),
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
			this->body = new std::stringstream();
		}
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   status_pages.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 12:14:45 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/10 12:36:38 by maiboyer         ###   ########.fr       */
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
#include "lib/Rc.hpp"
#include "lib/StringHelper.hpp"
#include "runtime/Logger.hpp"

Rc<Response> handlers::default_status_page(StatusCode code, bool with_body) {
	Rc<Response>		  res;
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
	if (with_body) {
		res->setBody(body.cast<std::istream>(), body->str().size());
	} else {
		res->setBody(Rc<std::stringstream>().cast<std::istream>(), body->str().size());
	}
	res->setMimeType("html");
	return res;
}

Rc<Response> Response::createStatusPageFor(Epoll&				 epoll,
										   Rc<Connection>&		 conn,
										   const config::Server* server,
										   StatusCode			 code,
										   bool					 with_body) {
	(void)(epoll);
	(void)(conn);
	std::stringstream s;
	s << code.code();
	std::string c = s.str();
	if (!(server == NULL || server->errors.count(c) == 0)) {
		try {
			std::size_t body_size = 0;
			std::string ext;
			std::string file_path;
			{
				std::vector<std::string> url_parts = Url(server->errors.at(c)).getParts();

				for (std::vector<std::string>::iterator it = url_parts.begin();
					 it != url_parts.end(); it++) {
					// no directory traversal for you :)
					if (it->empty())
						continue;
					if (*it == ".." || *it == ".")
						throw Request::PageException(status::SERVICE_UNAVAILABLE, with_body);
					file_path += "/";
					file_path += *it;
				}
				// remove all `//` and replace them with a single /
				for (std::string::size_type pos = file_path.find("//"); pos != std::string::npos;
					 pos						= file_path.find("//"))
					   file_path.replace(pos, 2, "/");
			}
			Rc<Response>	 res  = Rc<Response>();
			Rc<std::istream> body = getFileAt(file_path, server, NULL, &ext, &body_size, NULL);
			res->setBody(body, body_size);
			res->setMimeType(ext);
			res->setStatus(code);
			return res;
		} catch (...) {
			return (handlers::default_status_page(code, with_body));
		}
	}
	return (handlers::default_status_page(code, with_body));
}

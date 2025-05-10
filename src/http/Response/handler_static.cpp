/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_static.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 12:14:42 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/10 12:28:58 by maiboyer         ###   ########.fr       */
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

void handlers::handle_static_file(Epoll&		  epoll,
								  Rc<Connection>& connection,
								  Rc<Request>&	  req,
								  Rc<Response>&	  res) {
	(void)(epoll);
	if (req->getMethod() == "POST" || req->getMethod() == "PUT" || req->getMethod() == "DELETE") {
		handlers::handle_post_delete(epoll, connection, req, res);
		return;
	}
	try {
		LOG(info, "handling static_file for " << req->getUrl().getAll());
		std::size_t body_size = 0;
		std::string ext;
		std::string file_path;
		{
			std::vector<std::string>		url_parts	= req->getUrl().getParts();
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
		Rc<std::istream> body =
			getFileAt(file_path, req->getServer(), req->getRoute(), &ext, &body_size, NULL);
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

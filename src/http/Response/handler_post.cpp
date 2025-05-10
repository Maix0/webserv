/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_post.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 12:14:20 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/10 12:17:57 by maiboyer         ###   ########.fr       */
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

void handlers::handle_post_delete(Epoll&		  epoll,
								  Rc<Connection>& connection,
								  Rc<Request>&	  req,
								  Rc<Response>&	  res) {
	(void)(epoll);
	(void)(connection);
	LOG(info, "handling post_put_delete for " << req->getUrl().getAll());
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
	std::vector<std::string> url_parts = req->getUrl().getParts();
	url_parts.erase(url_parts.begin(),
					url_parts.begin() + (std::min(url_parts.size(), route.parts.size())));
	if (url_parts.empty()) {
		if (route.index.hasValue())
			url_parts.push_back(route.index.get());
		else
			url_parts.push_back("hello_yes_this_is_index");
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
		if (file_out.fail()) {
			LOG(warn, "Failed to open file " << file);
			throw Request::PageException(status::INTERNAL_SERVER_ERROR, req->getMethod() != "HEAD");
		}
		char*		  buffer = new char[COPY_BUFFER_SIZE];
		Rc<tiostream> body	 = req->getBody().get();
		body->seekg(0, std::ios_base::beg);
		if (body->fail()) {
			LOG(err, "failed to seekg");
		}
		body->seekp(0, std::ios_base::beg);
		if (body->fail()) {
			LOG(err, "failed to seekp");
		}
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

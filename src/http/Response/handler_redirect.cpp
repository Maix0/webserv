/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_redirect.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 12:14:40 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/10 12:15:50 by maiboyer         ###   ########.fr       */
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

void handlers::handle_redirect(Epoll&		   epoll,
							   Rc<Connection>& connection,
							   Rc<Request>&	   req,
							   Rc<Response>&   res) {
	(void)(epoll);
	(void)(connection);

	static const StatusCode CODE	  = status::MOVED_PERMANENTLY;
	const std::string		canonical = CODE.canonical().get_or("Unknown code");
	Rc<std::stringstream>	body;

	LOG(info, "handling redirect for " << req->getUrl().getAll());
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_cgi.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 12:14:38 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/10 12:20:31 by maiboyer         ###   ########.fr       */
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

void handlers::handle_cgi_request(Epoll&			 epoll,
								  Rc<Connection>&	 connection,
								  Rc<Request>&		 req,
								  Rc<Response>&		 res,
								  const config::Cgi& cgi,
								  std::string		 cgi_prefix) {
	(void)(epoll);
	(void)(connection);
	(void)(req);
	(void)(res);
	(void)(cgi);

	LOG(info, "handling cgi for " << req->getUrl().getAll());
	CgiList& cgi_list = State::getInstance().getCgis();
	assert(!cgi.binary.empty());
	Rc<CgiOutput> o = Rc<CgiOutput>(
		Functor6<CgiOutput, Epoll&, Rc<Request>&, const config::Cgi*, std::string&, Rc<Response>&,
				 Rc<Connection>&>(epoll, req, &cgi, cgi_prefix, res, connection),
		RCFUNCTOR);
	cgi_list.push_back(o);
	res->setCgi(o);
}


const config::Cgi* handlers::find_cgi_for(const Url&			 url,
								const config::Route& route,
								std::string&		 cgi_suffix) {
	std::map<std::string, config::Cgi>& all_cgis = State::getInstance().getConfig().cgi;
	std::vector<std::string>			parts	 = url.getParts();

	for (std::map<std::string, std::string>::const_iterator cit = route.cgi.begin();
		 cit != route.cgi.end(); cit++) {
		if (parts.size() > route.parts.size()) {
			if (string_ends_with(parts[route.parts.size()], cit->first)) {
				cgi_suffix = cit->first;
				return (&all_cgis.at(cit->second));
			}
		}
	}
	return NULL;
}

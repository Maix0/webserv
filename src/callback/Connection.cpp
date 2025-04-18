/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 18:56:10 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/18 12:12:00 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "app/State.hpp"
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "app/http/StatusCode.hpp"
#include "app/net/Connection.hpp"
#include "interface/Callback.hpp"
#include "runtime/EpollType.hpp"
#include "runtime/Logger.hpp"

// 4 MiB
#define MAX_READ_BYTES (1 << 22)

static char READ_BUF[MAX_READ_BYTES];

static void _send_builtin_code_response(Epoll&		   epoll,
										Rc<Callback>   self,
										Rc<Connection> inner,
										StatusCode	   code) {
	(void)(self);
	Rc<Response> res;
	LOG(info, "Returning page for code: " << code.code() << " - " << code.canonical());
	res =
		Response::createStatusPageFor(epoll, inner, inner->getRequest()->getServer(), code.code());

	inner->getRequest()->setFinished();
	inner->getRequest() =
		new Request(inner->getSocket()->getPort(), inner->getSocket()->getServer());
}

void _ConnCallbackR(Epoll& epoll, Rc<Callback> self, Rc<Connection> inner) {
	if (inner->isClosed()) {
		self->setFinished();
		return;
	}
	try {
		epoll.addCallback(inner->asFd(), READ, self);
		ssize_t res;
		if ((res = read(inner->asFd(), &READ_BUF, MAX_READ_BYTES)) < 0) {
			throw Request::PageException(500);
		}
		if (res > 0)
			inner->updateTime();
		inner->getInBuffer().insert(inner->getInBuffer().end(), &READ_BUF[0], &READ_BUF[res]);

		if (inner->getRequest()->parseBytes(inner->getInBuffer()))
			return (void)Response::createResponseFor(epoll, inner);
	} catch (const Request::PageException& e) {
		return _send_builtin_code_response(epoll, self, inner, e.statusCode());
	} catch (const std::exception& e) {
		LOG(warn, "parsing request: " << e.what());
		return _send_builtin_code_response(epoll, self, inner, 500);
	} catch (...) {
		LOG(warn, "parsing request: <unknown exception>");
		return _send_builtin_code_response(epoll, self, inner, 500);
	}
}

void _ConnCallbackW(Epoll& epoll, Rc<Callback> self, Rc<Connection> inner) {
	if (inner->isClosed()) {
		self->setFinished();
		return;
	}
	if (inner->getResponse()->isFinished())
		return;
	Connection::OBuffer& buf = inner->getOutBuffer();
	if (buf.size() < 4096) {
		std::size_t before = buf.size();
		buf.resize(4096);
		std::size_t asking = 4096 - before - 1;
		std::size_t got	   = inner->getResponse()->fill_buffer(&buf[before], asking);
		if (got < asking)
			inner->getResponse()->setFinished();
		buf.resize(before + got);
	}
	ssize_t res = 0;
	if ((res = write(inner->asFd(), &buf[0], buf.size())) < 0) {
		int serr = errno;
		(void)(serr);
		LOG(warn, "Error when reading...: " << strerror(serr));
		return;
	}
	LOG(debug, "adding back");
	epoll.addCallback(inner->asFd(), WRITE, self);
	if (res > 0) {
		inner->updateTime();
	}
	buf.erase(buf.begin(), buf.begin() + res);
}

void _ConnCallbackH(Epoll& epoll, Rc<Callback> self, Rc<Connection> inner) {
	(void)(self);
	self->setFinished();
	inner->setClosed();
	State&			ctx	 = State::getInstance();
	ConnectionList& conn = ctx.getConnections();

	while (true) {
		ConnectionList::iterator it = conn.begin();
		for (it = conn.begin(); it != conn.end() && (*it)->asFd() != inner->asFd(); it++)
			;
		if (it != conn.end()) {
			conn.erase(it);
		} else {
			break;
		}
	}
	epoll.removeCallback(inner->asFd(), WRITE);
	epoll.removeCallback(inner->asFd(), READ);
	epoll.removeCallback(inner->asFd(), HANGUP);
}

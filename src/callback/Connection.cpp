/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 18:56:10 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/02 15:02:43 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "interface/Callback.hpp"
#include "app/net/Connection.hpp"
#include "app/Context.hpp"
#include "runtime/EpollType.hpp"
#include "runtime/Logger.hpp"
#include "app/http/Request.hpp"

// 4 MiB
#define MAX_READ_BYTES (4 * 1024 * 1024)

static char READ_BUF[MAX_READ_BYTES];

static Request req;

void _ConnCallbackR(Epoll& epoll, Rc<Callback> self, Rc<Connection> inner) {
	if (inner->isClosed()) {
		self->setFinished();
		return;
	}
	epoll.addCallback(inner->asFd(), READ, self);
	ssize_t res;
	if ((res = read(inner->asFd(), &READ_BUF, MAX_READ_BYTES)) < 0) {
		LOG(warn, "Error when reading...");
		return;
	}
	inner->getInBuffer().insert(inner->getInBuffer().end(), &READ_BUF[0], &READ_BUF[res]);
	std::string possible_reponse;
	try {
		try {
			req.parseBytes(inner->getInBuffer());
		} catch (const Request::PageException& e) {
			LOG(info, "Early fail for request with code: " << e.statusCode());
			possible_reponse = Request::createStatusPageFor(e.statusCode());
			inner->getOutBuffer().insert(inner->getOutBuffer().end(), possible_reponse.begin(),
										 possible_reponse.end());
			{
				Rc<ConnectionCallback<WRITE> > con = new ConnectionCallback<WRITE>(inner);
				epoll.addCallback(inner->asFd(), WRITE, con.cast<Callback>());
			}
			req.setFinished();
			req = Request();
			return;
		} catch (const std::exception& e) {
			LOG(info, "Early fail for request 500: " << e.what());
			possible_reponse = Request::createStatusPageFor(500);
			inner->getOutBuffer().insert(inner->getOutBuffer().end(), possible_reponse.begin(),
										 possible_reponse.end());
			{
				Rc<ConnectionCallback<WRITE> > con = new ConnectionCallback<WRITE>(inner);
				epoll.addCallback(inner->asFd(), WRITE, con.cast<Callback>());
			}
			req.setFinished();
			req = Request();
			return;
		}
	} catch (const std::exception& e) {
		LOG(info, "Early fail for request 500: " << e.what());
		possible_reponse = Request::createStatusPageFor(500);
		inner->getOutBuffer().insert(inner->getOutBuffer().end(), possible_reponse.begin(),
									 possible_reponse.end());
		{
			Rc<ConnectionCallback<WRITE> > con = new ConnectionCallback<WRITE>(inner);
			epoll.addCallback(inner->asFd(), WRITE, con.cast<Callback>());
		}
		req.setFinished();
		req = Request();
		return;
	}
	LOG(info, "finished reading...");
}

void _ConnCallbackW(Epoll& epoll, Rc<Callback> self, Rc<Connection> inner) {
	if (inner->isClosed()) {
		self->setFinished();
		return;
	}
	LOG(info, "started writing...");
	Connection::Buffer& buf = inner->getOutBuffer();
	ssize_t				res = 0;
	if ((res = write(inner->asFd(), &buf[0], buf.size())) < 0) {
		LOG(warn, "Error when reading...");
		return;
	}
	buf.erase(buf.begin(), buf.begin() + res);
	epoll.addCallback(inner->asFd(), WRITE, self);
}

void _ConnCallbackH(Epoll& epoll, Rc<Callback> self, Rc<Connection> inner) {
	(void)(self);
	self->setFinished();
	inner->setClosed();
	Context&		ctx	 = Context::getInstance();
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 18:56:10 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 20:51:30 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "app/Callback.hpp"
#include "app/Connection.hpp"
#include "app/Context.hpp"
#include "app/Logger.hpp"

// 4 kiB
#define MAX_READ_BYTES	(4 * 1024)
// 1 kiB
#define MAX_WRITE_BYTES (1 * 1024)

namespace app {
	void _ConnCallbackR(Epoll& epoll, Shared<Callback> self, Shared<Connection> inner) {
		if (inner->isClosed())
			return;
		epoll.addCallback(inner->asFd(), Epoll::READ, self);
		char	buffer[MAX_READ_BYTES + 1] = {};
		ssize_t res;
		if ((res = read(inner->asFd(), &buffer, MAX_READ_BYTES)) < 0) {
			LOG(warn, "Error when reading...");
			return;
		}
		if (res != 0) {
			inner->getBuffer().insert(inner->getBuffer().end(), &buffer[0], &buffer[res]);

			Shared<ConnectionCallback<Epoll::WRITE> > cb =
				new ConnectionCallback<Epoll::WRITE>(inner);
			epoll.addCallback(inner->asFd(), Epoll::WRITE, cb.cast<Callback>());
		}
	}

	void _ConnCallbackW(Epoll& epoll, Shared<Callback> self, Shared<Connection> inner) {
		if (inner->isClosed())
			return;
		Connection::Buffer& buf = inner->getBuffer();
		ssize_t				res = 0;
		if ((res = write(inner->asFd(), &buf[0], buf.size())) < 0) {
			LOG(warn, "Error when reading...");
			return;
		}
		buf.erase(buf.begin(), buf.begin() + res);
		if (!buf.empty())
			epoll.addCallback(inner->asFd(), Epoll::WRITE, self);
	}

	void _ConnCallbackH(Epoll& epoll, Shared<Callback> self, Shared<Connection> inner) {
		(void)(self);
		inner->setClosed();
		Context&		ctx	 = Context::getInstance();
		ConnectionList& conn = ctx.getConnections();

		while (true) {
			ConnectionList::iterator it = conn.begin();
			for (it = conn.begin(); it != conn.end(); it++) {
				if ((*it)->asFd() == inner->asFd())
					break;
			}
			if (it != conn.end()) {
				conn.erase(it);
			} else {
				break;
			}
		}
		epoll.removeCallback(inner->asFd(), Epoll::WRITE);
		epoll.removeCallback(inner->asFd(), Epoll::READ);
		epoll.removeCallback(inner->asFd(), Epoll::HANGUP);
	}
}  // namespace app

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 14:28:15 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 20:58:04 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Socket.hpp"
#include "app/Callback.hpp"
#include "app/Connection.hpp"
#include "app/Context.hpp"
#include "app/Epoll.hpp"
#include "app/Logger.hpp"
#include "app/Shared.hpp"

#include <fcntl.h>
#include <sys/socket.h>
namespace app {
	void SocketCallback::call(Epoll& epoll, Shared<Callback> self) {
		LOG(debug, "Called for socket: " << this->socketfd->asFd());
		// add us back to the callback queue
		epoll.addCallback(this->socketfd->asFd(), Epoll::READ, self);

		struct sockaddr_storage addr;
		socklen_t				addr_size = 0;

		int res = accept(this->socketfd->asFd(), (struct sockaddr*)&addr, &addr_size);
		if (res == -1) {
			LOG(err, "Failed to accept connection for socket " << this->socketfd->asFd());
			return;
		}
		fcntl(res, FD_CLOEXEC);
		fcntl(res, F_SETFL, O_NONBLOCK);

		Context&		   ctx	= Context::getInstance();
		Shared<Connection> conn = new Connection(res);
		ctx.getConnections().push_back(conn);
		{
			Shared<ConnectionCallback<Epoll::READ> > cb = new ConnectionCallback<Epoll::READ>(conn);
			epoll.addCallback(res, Epoll::READ, cb.cast<Callback>());
		}
		{
			Shared<ConnectionCallback<Epoll::HANGUP> > cb =
				new ConnectionCallback<Epoll::HANGUP>(conn);
			epoll.addCallback(res, Epoll::HANGUP, cb.cast<Callback>());
		}
	};
};	// namespace app

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 14:28:15 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/13 16:24:44 by maiboyer         ###   ########.fr       */
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
		epoll.addCallback(this->socketfd->asFd(), READ, self);

		struct sockaddr_storage addr;
		socklen_t				addr_size = 0;

		int res = accept(this->socketfd->asFd(), (struct sockaddr*)&addr, &addr_size);
		if (res == -1) {
			LOG(err, "Failed to accept connection for socket " << this->socketfd->asFd());
			return;
		}
		{
			int flags = fcntl(res, F_GETFL);
			fcntl(res, F_SETFL, flags | O_NONBLOCK);
			fcntl(res, FD_CLOEXEC);
		}

		Context&		   ctx	= Context::getInstance();
		Shared<Connection> conn = new Connection(res);
		ctx.getConnections().push_back(conn);
		{
			Shared<ConnectionCallback<READ> > cb = new ConnectionCallback<READ>(conn);
			epoll.addCallback(res, READ, cb.cast<Callback>());
		}
		{
			Shared<ConnectionCallback<HANGUP> > cb = new ConnectionCallback<HANGUP>(conn);
			epoll.addCallback(res, HANGUP, cb.cast<Callback>());
		}
	};
};	// namespace app

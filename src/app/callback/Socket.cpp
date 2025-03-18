/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 14:28:15 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/15 09:53:15 by maiboyer         ###   ########.fr       */
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
#include <netinet/in.h>
#include <sys/socket.h>

namespace app {
	void SocketCallback::call(Epoll& epoll, Shared<Callback> self) {
		LOG(debug, "Called for socket: " << this->socketfd->asFd());
		// add us back to the callback queue
		epoll.addCallback(this->socketfd->asFd(), READ, self);

		struct sockaddr_storage addr;
		socklen_t				addr_size = sizeof(addr);

		int res = accept(this->socketfd->asFd(), (struct sockaddr*)&addr, &addr_size);
		if (res == -1) {
			LOG(err, "Failed to accept connection for socket " << this->socketfd->asFd());
			return;
		}
		{
			int flags;
			_ERR_RET((flags = fcntl(res, F_GETFL)));
			_ERR_RET(fcntl(res, F_SETFL, flags | O_NONBLOCK));
			_ERR_RET(fcntl(res, FD_CLOEXEC));
		}
		if (addr.ss_family != AF_INET) {
			LOG(warn, "Dropped connection " << res << " due to it not being IPv4");
			close(res);
			return;
		}
		struct sockaddr_in* addr_ip = (struct sockaddr_in*)(&addr);

		Context&		   ctx		= Context::getInstance();
		Shared<Connection> conn		= new Connection(res, Ip(ntohl(addr_ip->sin_addr.s_addr)),
													 Port(ntohs(addr_ip->sin_port)));
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

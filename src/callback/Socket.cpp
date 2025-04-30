/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 14:28:15 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/30 23:35:26 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/net/Socket.hpp"
#include "app/State.hpp"
#include "app/net/Connection.hpp"
#include "interface/Callback.hpp"
#include "lib/Rc.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/Logger.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

void SocketCallback::call(Epoll& epoll, Rc<Callback> self) {
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

	State&		   ctx			= State::getInstance();
	Rc<Connection> conn(Functor4<Connection, int, Ip, Port, Rc<Socket>&>(
							res, Ip(ntohl(addr_ip->sin_addr.s_addr)),
							Port(ntohs(addr_ip->sin_port)), this->socketfd),
						RCFUNCTOR);
	ctx.getConnections().push_back(conn);

	Rc<ConnectionCallback<READ> > rcb = Rc<ConnectionCallback<READ> >(
		Functor1<ConnectionCallback<READ>, Rc<Connection>& >(conn), RCFUNCTOR);
	Rc<ConnectionCallback<HANGUP> > hcb = Rc<ConnectionCallback<HANGUP> >(
		Functor1<ConnectionCallback<HANGUP>, Rc<Connection>& >(conn), RCFUNCTOR);
	epoll.addCallback(res, READ, rcb.cast<Callback>());
	epoll.addCallback(res, HANGUP, hcb.cast<Callback>());
};

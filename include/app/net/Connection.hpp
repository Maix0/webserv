/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 18:43:37 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 13:22:24 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include "app/net/Socket.hpp"
#include "interface/AsFd.hpp"
#include "lib/Rc.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/Logger.hpp"

class Connection : public AsFd {
	public:
		struct State {
				enum Inner {
					NoState,
					NewRequest,
					Headers,
					Body,
					Error,
				};
		};

		typedef std::string	 Buffer;
		typedef State::Inner StateE;

	private:
		StateE state;
		int	   fd;

		Buffer inbuffer;
		Buffer outbuffer;

		bool closed;
		Ip	 remote_ip;
		Port remote_port;

	public:
		virtual ~Connection() {
			if (this->fd != -1)
				close(this->fd);
			LOG(debug, "closing connection " << fd << " for " << remote_ip << ":" << remote_port);
		};
		Connection(int fd, Ip ip, Port port)
			: fd(fd), closed(false), remote_ip(ip), remote_port(port) {
			LOG(debug, "new connection " << fd << " for " << ip << ":" << port);
		};

		Buffer& getInBuffer() { return this->inbuffer; };
		Buffer& getOutBuffer() { return this->outbuffer; };
		Ip		getIp() { return this->remote_ip; };
		Port	getPort() { return this->remote_port; };

		StateE getState() { return this->state; };
		void   setState(StateE state) { this->state = state; };

		int	 asFd() { return this->fd; };
		bool isClosed() { return this->closed; };
		void setClosed() { this->closed = true; };
};

void _ConnCallbackR(Epoll& epoll, Rc<Callback> self, Rc<Connection> inner);
void _ConnCallbackW(Epoll& epoll, Rc<Callback> self, Rc<Connection> inner);
void _ConnCallbackH(Epoll& epoll, Rc<Callback> self, Rc<Connection> inner);

template <EpollType TY>
class ConnectionCallback : public Callback {
	private:
		Rc<Connection> inner;

	public:
		virtual ~ConnectionCallback() {};
		ConnectionCallback(Rc<Connection> inner) : inner(inner) {};
		int		  getFd() { return this->inner->asFd(); };
		EpollType getTy() { return TY; };

		void call(Epoll& epoll, Rc<Callback> self) {
			if (TY == READ)
				return _ConnCallbackR(epoll, self, this->inner);
			else if (TY == WRITE)
				return _ConnCallbackW(epoll, self, this->inner);
			else if (TY == HANGUP)
				return _ConnCallbackH(epoll, self, this->inner);
			else
				throw std::runtime_error("Unknown ConnectionCallback Type");
		}
};

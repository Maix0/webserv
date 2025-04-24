/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 18:43:37 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/23 12:37:35 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <cstddef>
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "app/net/Socket.hpp"
#include "interface/AsFd.hpp"
#include "lib/Rc.hpp"
#include "lib/Time.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/Logger.hpp"

class Connection : public AsFd {
	public:
		typedef std::string		  IBuffer;
		typedef std::vector<char> OBuffer;

		static const std::size_t KEEP_ALIVE_TIMEOUT = 5;

	private:
		int fd;

		IBuffer inbuffer;
		OBuffer outbuffer;

		bool closed;
		Ip	 remote_ip;
		Port remote_port;

		Rc<Socket>	 socket;
		Rc<Request>	 request;
		Rc<Response> response;

		Time last_updated;

	public:
		virtual ~Connection() {
			if (this->fd != -1)
				close(this->fd);
			LOG(debug, "closing connection " << fd << " for " << remote_ip << ":" << remote_port);
		};
		Connection(int fd, Ip ip, Port port, Rc<Socket> sock)
			: fd(fd),
			  closed(false),
			  remote_ip(ip),
			  remote_port(port),
			  socket(sock),
			  request(new Request(ip, port, socket->getServer())),
			  last_updated(Time::now()) {
			LOG(debug, "new connection " << fd << " for " << ip << ":" << port);
			this->response->setFinished();
		};

		IBuffer& getInBuffer() { return this->inbuffer; };
		OBuffer& getOutBuffer() { return this->outbuffer; };
		Ip		 getIp() { return this->remote_ip; };
		Port	 getPort() { return this->remote_port; };

		Rc<Socket>	  getSocket() { return this->socket; };
		Rc<Request>&  getRequest() { return this->request; };
		Rc<Response>& getResponse() { return this->response; };

		int	 asFd() { return this->fd; };
		bool isClosed() { return this->closed; };
		void setClosed() { this->closed = true; };
		void updateTime() {
			this->last_updated = Time::now();
		};
		bool timeout() const {
			Time should_to;
			should_to.inner = this->last_updated.inner + Connection::KEEP_ALIVE_TIMEOUT;
			return should_to <= Time::now();
		}
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

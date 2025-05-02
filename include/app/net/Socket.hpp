/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 13:36:52 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/02 10:45:28 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <ostream>

#include "app/net/Ip.hpp"
#include "app/net/Port.hpp"
#include "config/Config.hpp"
#include "interface/AsFd.hpp"
#include "interface/Callback.hpp"
#include "lib/Rc.hpp"

class Socket : public AsFd {
	private:
		int				fd;
		Port			port;
		Port			bound_port;
		Ip				host;
		config::Server* server;

	public:
		static const int BACKLOG = 5;

		Socket();
		Socket(const Ip& host, Port port, config::Server* server = NULL);
		virtual ~Socket();

		virtual int asFd() { return (this->fd); };

		int				getFd() { return (this->fd); };
		Port			getPort() { return (this->port); };
		Port			getBoundPort() { return (this->bound_port); };
		Ip				getHost() { return (this->host); };
		config::Server* getServer() { return (this->server); }
};

class SocketCallback : public Callback {
	private:
		Rc<Socket> socketfd;

	public:
		virtual ~SocketCallback() {};
		SocketCallback(Rc<Socket>& s) : socketfd(s) {};
		SocketCallback(const SocketCallback& rhs) : socketfd(rhs.socketfd) {};
		SocketCallback& operator=(const SocketCallback& rhs) {
			if (this != &rhs)
				this->socketfd = rhs.socketfd;
			return (*this);
		};
		void	  call(Epoll& epoll, Rc<Callback> self);
		int		  getFd() { return this->socketfd->asFd(); };
		EpollType getTy() { return READ; };
};

class ShutdownCallback : public Callback {
	private:
		Rc<Socket> socketfd;

	public:
		virtual ~ShutdownCallback() {};
		ShutdownCallback(Rc<Socket>& s) : socketfd(s) {};
		ShutdownCallback(const ShutdownCallback& rhs) : socketfd(rhs.socketfd) {};
		ShutdownCallback& operator=(const ShutdownCallback& rhs) {
			if (this != &rhs)
				this->socketfd = rhs.socketfd;
			return (*this);
		};
		void call(Epoll& epoll, Rc<Callback> self);

		int		  getFd() { return this->socketfd->asFd(); };
		EpollType getTy() { return READ; };
};

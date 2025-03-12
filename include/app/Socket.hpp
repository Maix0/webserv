/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 13:36:52 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 18:18:24 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <ostream>
#include <string>
#include "app/AsFd.hpp"
#include "app/Callback.hpp"
#include "app/Shared.hpp"

namespace app {
	struct Ip {
		unsigned int inner;

		Ip() : inner(0) {};
		Ip(unsigned int raw) : inner(raw) {};
		~Ip() {};
		Ip(const Ip& rhs) : inner(rhs.inner) {};
		Ip& operator=(const Ip& rhs) {
			if (this != &rhs)
				this->inner = rhs.inner;
			return (*this);
		};

		bool				 operator==(const Ip& rhs) const { return (this->inner == rhs.inner); };
		bool				 operator>(const Ip& rhs) const { return (this->inner > rhs.inner); };

		bool				 operator<(const Ip& rhs) const { return (rhs > *this); };
		bool				 operator<=(const Ip& rhs) const { return !(*this > rhs); };
		bool				 operator>=(const Ip& rhs) const { return !(*this < rhs); };
		bool				 operator!=(const Ip& rhs) const { return !(*this == rhs); };

		friend std::ostream& operator<<(std::ostream& o, const Ip& rhs) {
			unsigned char* ptr = (unsigned char*)&rhs.inner;
			o << (unsigned int)ptr[0] << "." << (unsigned int)ptr[1] << "." << (unsigned int)ptr[2]
			  << "." << (unsigned int)ptr[3];

			return (o);
		}
	};

	struct Port {
		unsigned short inner;
		Port() : inner(0) {};
		Port(unsigned short raw) : inner(raw) {};
		~Port() {};
		Port(const Port& rhs) : inner(rhs.inner) {};
		Port& operator=(const Port& rhs) {
			if (this != &rhs)
				this->inner = rhs.inner;
			return (*this);
		};

		bool operator==(const Port& rhs) const { return (this->inner == rhs.inner); };
		bool operator>(const Port& rhs) const { return (this->inner > rhs.inner); };

		bool operator<(const Port& rhs) const { return (rhs > *this); };
		bool operator<=(const Port& rhs) const { return !(*this > rhs); };
		bool operator>=(const Port& rhs) const { return !(*this < rhs); };
		bool operator!=(const Port& rhs) const { return !(*this == rhs); };

		friend std::ostream& operator<<(std::ostream& o, const Port& rhs) {
			return (o << rhs.inner);
		}
	};

	class Socket : public app::AsFd {
	private:
		int	 fd;
		Port port;
		Port bound_port;
		Ip	 host;

	public:
		static const int BACKLOG = 5;

		Socket();
		Socket(const Ip& host, Port port);
		virtual ~Socket();

		virtual int asFd() { return (this->fd); };

		int			getFd() { return (this->fd); };
		Port		getPort() { return (this->port); };
		Port		getBoundPort() { return (this->bound_port); };
		Ip			getHost() { return (this->host); };
	};

	class SocketCallback : public app::Callback {
	private:
		Shared<Socket> socketfd;

	public:
		virtual ~SocketCallback() {};
		SocketCallback(Shared<Socket> s) : socketfd(s) {};
		SocketCallback(const SocketCallback& rhs) : socketfd(rhs.socketfd) {};
		SocketCallback& operator=(const SocketCallback& rhs) {
			if (this != &rhs)
				this->socketfd = rhs.socketfd;
			return (*this);
		};
		void call(Epoll& epoll, Shared<Callback> self);
	};

	class ShutdownCallback : public app::Callback {
	private:
		Shared<Socket> socketfd;
		Shared<bool>   shutdown;

	public:
		virtual ~ShutdownCallback() {};
		ShutdownCallback(Shared<Socket> s, Shared<bool> shutdown)
			: socketfd(s), shutdown(shutdown) {};
		ShutdownCallback(const ShutdownCallback& rhs)
			: socketfd(rhs.socketfd), shutdown(rhs.shutdown) {};
		ShutdownCallback& operator=(const ShutdownCallback& rhs) {
			if (this != &rhs) {
				this->socketfd = rhs.socketfd;
				this->shutdown = rhs.shutdown;
			}
			return (*this);
		};
		void call(Epoll& epoll, Shared<Callback> self);
	};
};	// namespace app

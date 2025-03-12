/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 18:43:37 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 19:57:10 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <deque>
#include "app/Epoll.hpp"
#include "app/Logger.hpp"
#include "app/Shared.hpp"
#include "app/Socket.hpp"
namespace app {
	class Connection : public AsFd {
	public:
		typedef std::vector<char> Buffer;

	private:
		int	   fd;
		Buffer rw_buffer;
		bool   closed;

	public:
		virtual ~Connection() {
			if (this->fd != -1)
				close(this->fd);
			LOG(info, "Closing connection " << this->fd);
		};
		Connection(int fd) : fd(fd), closed(false) {};

		Buffer& getBuffer() { return this->rw_buffer; };
		int		asFd() { return this->fd; };
		bool	isClosed() { return this->closed; };
		void	setClosed() { this->closed = true; }
	};

	void _private_ConnCallbackR(Epoll& epoll, Shared<Callback> self, Shared<Connection> inner);
	void _private_ConnCallbackW(Epoll& epoll, Shared<Callback> self, Shared<Connection> inner);
	void _private_ConnCallbackH(Epoll& epoll, Shared<Callback> self, Shared<Connection> inner);

	template <Epoll::EpollType TY>
	class ConnectionCallback : public Callback {
	private:
		Shared<Connection> inner;

	public:
		virtual ~ConnectionCallback() {};
		ConnectionCallback(Shared<Connection> inner) : inner(inner) {};

		void call(Epoll& epoll, Shared<Callback> self) {
			if (TY == Epoll::READ)
				return _private_ConnCallbackR(epoll, self, this->inner);
			else if (TY == Epoll::WRITE)
				return _private_ConnCallbackW(epoll, self, this->inner);
			else if (TY == Epoll::HANGUP)
				return _private_ConnCallbackH(epoll, self, this->inner);
			else
				throw std::runtime_error("Unknown ConnectionCallback Type");
		}
	};

};	// namespace app

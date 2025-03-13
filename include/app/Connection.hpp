/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 18:43:37 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/13 17:13:55 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include "app/AsFd.hpp"
#include "app/Epoll.hpp"
#include "app/File.hpp"
#include "app/Logger.hpp"
#include "app/Shared.hpp"

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

	void _ConnCallbackR(Epoll& epoll, Shared<Callback> self, Shared<Connection> inner);
	void _ConnCallbackW(Epoll& epoll, Shared<Callback> self, Shared<Connection> inner);
	void _ConnCallbackH(Epoll& epoll, Shared<Callback> self, Shared<Connection> inner);

	template <EpollType TY>
	class ConnectionCallback : public Callback {
	private:
		Shared<Connection>		  inner;

	public:

		virtual ~ConnectionCallback() {};
		ConnectionCallback(Shared<Connection> inner) : inner(inner) {};
		int		  getFd() { return this->inner->asFd(); };
		EpollType getTy() { return TY; };

		void	  call(Epoll& epoll, Shared<Callback> self) {
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

};	// namespace app

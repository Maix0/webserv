/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 13:39:20 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/13 12:25:12 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Socket.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sstream>
#include <stdexcept>

#include "app/Logger.hpp"

namespace app {
	Socket::Socket() : fd(-1), port(0) {}

	Socket::Socket(const Ip& host, Port port) {
		this->fd				= -1;
		this->host				= host;
		this->port				= port;

		struct sockaddr_in addr = {};

		addr.sin_port			= htons(port.inner);
		addr.sin_addr.s_addr	= htonl(host.inner);
		addr.sin_family			= AF_INET;

		int sockfd				= socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
			throw std::runtime_error("unable to create socket");

		this->fd = sockfd;
		LOG(debug, "new socket " << this->fd << " for " << host << ":" << port);
		{
			int flags = fcntl(this->fd, F_GETFL);
			fcntl(this->fd, F_SETFL, flags | O_NONBLOCK);
			fcntl(this->fd, FD_CLOEXEC);
		}
		int r	 = (bind(this->fd, (struct sockaddr*)&addr, sizeof(addr)));
		int serr = errno;
		(void)(serr);  // used by the log macro
		if (r != 0) {
			LOG(err, "failed to bind socket " << this->fd << "(" << host << ":" << port
											  << "): " << strerror(serr));
			throw std::runtime_error("unable to bind socket");
		} else {
			if (this->port.inner == 0) {
				struct sockaddr_in s	 = {};
				unsigned int	   ssize = sizeof(s);
				if (getsockname(this->fd, (struct sockaddr*)&s, &ssize) == 0) {
					LOG(trace, "getsockname: " << s.sin_port);
					this->bound_port = Port(ntohs(s.sin_port));
				} else {
					LOG(err, "failed to get sockname on socket " << this->fd);
					throw std::runtime_error("getsockname error");
				}

			} else
				this->bound_port = this->port;
			LOG(debug, "successfully bound socket "
						   << this->fd << " onto " << this->host << ":" << this->bound_port
						   << (this->port != this->bound_port ? " (OS-allocated port)" : ""));
		}

		if (listen(this->fd, BACKLOG) != 0) {
			LOG(err, "failed to lisen on socket " << this->fd << "(" << this->host << ":"
												  << this->bound_port << "): " << strerror(serr));
			throw std::runtime_error("unable to listen on socket");
		} else {
			LOG(trace, "lisening on socket "
						   << this->fd << " (" << this->host << ":" << this->bound_port
						   << (this->port != this->bound_port ? " OS-allocated port" : "") << ")");
		}
	};

	Socket::~Socket() {
		LOG(trace, "dropping socket " << this->fd);
		if (this->fd != -1)
			close(this->fd);
	}
}  // namespace app

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 13:39:20 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/04 14:47:22 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Socket.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include "app/Logger.hpp"

#define BACKLOG 5

std::ostream& operator<<(std::ostream& o, const Ip& rhs) {
	unsigned char* ptr = (unsigned char*)&rhs.inner;
	o << (unsigned int)ptr[0] << "." << (unsigned int)ptr[1] << "." << (unsigned int)ptr[2] << "." << (unsigned int)ptr[3];

	return (o);
}

Socket::Socket() : fd(-1), port(0) {}

Socket::Socket(const Ip& host, unsigned short port) {
	this->fd   = -1;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		throw std::runtime_error("unable to create socket");
	this->fd = sockfd;
	LOG(debug, "new socket " << this->fd << " for " << host << ":" << port);

	struct sockaddr_in servaddr = {};
	servaddr.sin_family			= AF_INET;
	servaddr.sin_port			= htonl(port);
	servaddr.sin_addr.s_addr	= htonl(host.inner);

	if ((bind(this->fd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
		LOG(err, "failed to bind socket " << this->fd << "(" << host << ":" << port
										  << "): " << strerror(errno));
		throw std::runtime_error("unable to bind socket");
	} else
		LOG(debug, "successfully bound socket " << this->fd << "onto " << host << ":" << port);

	if (listen(this->fd, BACKLOG) != 0)
		LOG(err, "failed to lisen on socket " << this->fd << "(" << host << ":" << port
											  << "): " << strerror(errno));
	else
		LOG(debug, "lisening on socket " << this->fd << "(" << host << ":" << port);
};

Socket::~Socket() {
	if (this->fd != -1)
		close(this->fd);
}

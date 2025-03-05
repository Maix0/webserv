/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 13:39:20 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/05 16:23:12 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Socket.hpp"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>

#include "app/Logger.hpp"

#define BACKLOG 5

Socket::Socket() : fd(-1), port(0) {}

Socket::Socket(const std::string& host, Port port) : port(port) {
	this->fd	   = -1;
	this->host_str = host;

	struct addrinfo* res;

	struct addrinfo	 hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family	   = AF_INET;	   // Allow IPv4
	hints.ai_socktype  = SOCK_STREAM;  // TCP
	hints.ai_flags	   = AI_PASSIVE;   // For wildcard IP address
	hints.ai_protocol  = 0;			   // Any protocaol
	hints.ai_canonname = NULL;
	hints.ai_addr	   = NULL;
	hints.ai_next	   = NULL;

	if (getaddrinfo(this->host_str.c_str(), NULL, &hints, &res) != 0) {
		int serr = errno;
		LOG(err, "failed to lookup host for " << this->host_str << ":" << gai_strerror(serr));
		throw std::runtime_error("addrinfo failed");
	} else {
		LOG(debug, "addrinfo for " << this->host_str);
	}
	if (res == NULL) {
		LOG(err, "failed to lookup host for " << this->host_str);
	}
	struct sockaddr_in* test = (struct sockaddr_in*)res;
	test->sin_port			 = htonl(port.inner);
	this->host_ip			 = ntohl(test->sin_addr.s_addr);
	LOG(debug, "host: '" << this->host_str << "' = " << this->host_ip);

	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1)
		throw std::runtime_error("unable to create socket");

	this->fd = sockfd;
	LOG(debug, "new socket " << this->fd << " for " << host << ":" << port);

	int r	 = (bind(this->fd, res->ai_addr, res->ai_addrlen));
	int serr = errno;
	freeaddrinfo(res);
	if (r != 0) {
		LOG(err, "failed to bind socket " << this->fd << "(" << host << ":" << port
										  << "): " << strerror(serr));
		throw std::runtime_error("unable to bind socket");
	} else
		LOG(debug, "successfully bound socket " << this->fd << " onto " << host << ":" << port);

	if (listen(this->fd, BACKLOG) != 0)
		LOG(err, "failed to lisen on socket " << this->fd << "(" << host << ":" << port
											  << "): " << strerror(serr));
	else
		LOG(debug, "lisening on socket " << this->fd << " (" << host << ":" << port << ")");
};

Socket::~Socket() {
	if (this->fd != -1)
		close(this->fd);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:07:56 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/08 18:25:39 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Epoll.hpp"
#include "app/Logger.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>

namespace app {

	Epoll::Epoll() {
		this->fd = epoll_create(1);
		if (this->fd == -1)
			throw std::runtime_error("epoll_create failed");
		if (fcntl(this->fd, FD_CLOEXEC) != 0)
			throw std::runtime_error("epoll fcntl FD_CLOEXEC");
	}

	Epoll::~Epoll() {
		if (this->fd != -1)
			close(this->fd);
	}
	void Epoll::addFd(Shared<AsFd> fd) {
		struct epoll_event ev;
		ev.data.fd = fd->asFd();
		ev.events  = EPOLLIN;
		if (epoll_ctl(this->fd, EPOLL_CTL_ADD, fd->asFd(), &ev) == -1) {
			int serrno = errno;
			LOG(err, "epoll_add: failed to add fd " << fd->asFd() << ":" << strerror(serrno));
		}
	}

	void Epoll::removeFd(Shared<AsFd> fd) {
		(void)(fd);
	}

}  // namespace app

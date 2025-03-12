/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:07:56 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 15:07:23 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Epoll.hpp"
#include "app/Callback.hpp"
#include "app/Logger.hpp"
#include "app/Socket.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <cstddef>
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
	bool Epoll::addCallback(int fd, int eventType, Shared<Callback> callback) {
		struct epoll_event ev;
		ev.data.fd = fd;
		ev.events  = eventType;
		if (epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
			int serrno = errno;
			(void)(serrno);	 // log macro
			LOG(err, "epoll_add: failed to add callback for fd" << fd << ":" << strerror(serrno));
			return false;
		}
		this->callbacks.insert(std::make_pair(fd, callback));
		return true;
	}

	bool Epoll::removeCallback(int fd) {
		std::map<int, Shared<Callback> >::iterator e = this->callbacks.find(fd);
		if (e == this->callbacks.end())
			return false;
		this->callbacks.erase(e);
		struct epoll_event ev;
		ev.data.fd = fd;
		ev.events  = EPOLLIN | EPOLLOUT;
		if (epoll_ctl(this->fd, EPOLL_CTL_DEL, fd, &ev) == -1) {
			int serrno = errno;
			(void)(serrno);	 // log macro
			LOG(err, "epoll_add: failed to remove fd " << fd << ":" << strerror(serrno));
			return false;
		}
		return true;
	}

	std::vector<std::pair<EpollEvent, Shared<Callback> > > Epoll::fetchCallbacks() {
		std::vector<std::pair<EpollEvent, Shared<Callback> > > out;
		struct epoll_event									   events[MAX_EVENTS] = {};
		int event_count = epoll_wait(this->fd, events, MAX_EVENTS, EPOLL_TIMEOUT);
		if (event_count == -1) {
			int serrno = errno;
			(void)(serrno);	 // log macro
			LOG(fatal, "failed to epoll_wait: " << strerror(serrno));
			throw std::runtime_error("epoll fail");
		}
		out.reserve(event_count + 1);
		for (int i = 0; i < event_count; i++) {
			EpollEvent eevent;
			eevent.read									  = events[i].events & EPOLLIN;
			eevent.write								  = events[i].events & EPOLLOUT;
			std::map<int, Shared<Callback> >::iterator it = this->callbacks.find(events[i].data.fd);
			if (it != this->callbacks.end())
				out.push_back(std::make_pair(eevent, it->second));
		}
		return out;
	}
}  // namespace app

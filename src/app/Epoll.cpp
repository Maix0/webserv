/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:07:56 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 20:00:06 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Epoll.hpp"
#include "app/Callback.hpp"
#include "app/Logger.hpp"
#include "app/Shared.hpp"
#include "app/Socket.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <ios>
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
	bool Epoll::addCallback(int fd, EpollType eventType, Shared<Callback> callback) {
		bool new_ = false;
		if (this->callbacks.count(fd) == 0) {
			this->callbacks[fd] = EpollCallback();
			new_				= true;
		}
		EpollCallback& e = this->callbacks.find(fd)->second;
		if (eventType & READ)
			e.read.insert(callback);
		if (eventType & WRITE)
			e.write.insert(callback);
		if (eventType & HANGUP)
			e.hangup.insert(callback);

		struct epoll_event ev  = {};
		ev.data.fd			   = fd;
		ev.events			  |= e.write.hasValue() ? EPOLLOUT : 0;
		ev.events			  |= e.read.hasValue() ? EPOLLIN : 0;
		ev.events			  |= e.hangup.hasValue() ? (EPOLLHUP | EPOLLRDHUP) : 0;

		int op				   = new_ ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

		if (epoll_ctl(this->fd, op, fd, &ev) == -1) {
			int serrno = errno;
			(void)(serrno);	 // log macro
			LOG(err, "epoll_ctl: " << fd << ":" << strerror(serrno));
			return false;
		}
		return true;
	}

	bool Epoll::removeCallback(int fd, EpollType eventType) {
		if (this->callbacks.count(fd) == 0)
			return false;
		EpollCallback& e = this->callbacks.find(fd)->second;
		if (eventType & READ)
			e.read.remove();
		if (eventType & WRITE)
			e.write.remove();
		if (eventType & HANGUP)
			e.hangup.remove();

		struct epoll_event ev  = {};
		ev.data.fd			   = fd;
		ev.events			  |= e.write.hasValue() ? EPOLLOUT : 0;
		ev.events			  |= e.read.hasValue() ? EPOLLIN : 0;
		ev.events			  |= e.hangup.hasValue() ? (EPOLLHUP | EPOLLRDHUP) : 0;

		int op				   = (ev.events == 0) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;

		if (epoll_ctl(this->fd, op, fd, &ev) == -1) {
			int serrno = errno;
			(void)(serrno);	 // log macro
			LOG(err, "epoll_ctl: " << fd << ":" << strerror(serrno));
			return false;
		}

		if (op == EPOLL_CTL_DEL)
			this->callbacks.erase(this->callbacks.find(fd));

		return true;
	}

	std::vector<Shared<Callback> > Epoll::fetchCallbacks() {
		std::vector<Shared<Callback> > out;
		struct epoll_event			   events[MAX_EVENTS] = {};
		int event_count = epoll_wait(this->fd, events, MAX_EVENTS, EPOLL_TIMEOUT);
		if (event_count == -1) {
			if (errno == EINTR)
				return out;
			int serrno = errno;
			(void)(serrno);	 // log macro
			LOG(fatal, "failed to epoll_wait: " << strerror(serrno));
			throw std::runtime_error("epoll fail");
		}
		out.reserve(event_count + 1);
		for (int i = 0; i < event_count; i++) {
			CallbackStorage::iterator it = this->callbacks.find(events[i].data.fd);
			if (it != this->callbacks.end()) {
				if (events[i].events & EPOLLIN && it->second.read.hasValue()) {
					out.push_back(it->second.read.get());
					this->removeCallback(it->first, READ);
				}
				if (events[i].events & EPOLLOUT && it->second.write.hasValue()) {
					out.push_back(it->second.write.get());
					this->removeCallback(it->first, WRITE);
				}
				if (events[i].events & (EPOLLHUP | EPOLLRDHUP) && it->second.hangup.hasValue()) {
					out.push_back(it->second.hangup.get());
					this->removeCallback(it->first, HANGUP);
				}
			}
		}
		return out;
	}
}  // namespace app

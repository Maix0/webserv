/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:07:56 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/02 14:14:10 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "runtime/Epoll.hpp"
#include "interface/Callback.hpp"
#include "runtime/Logger.hpp"
#include "lib/Rc.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>

Epoll::Epoll() {
	this->fd = epoll_create(1);
	if (this->fd == -1)
		throw std::runtime_error("epoll_create failed");
	{
		int flags;
		_ERR_RET_THROW((flags = fcntl(this->fd, F_GETFL)));

		_ERR_RET_THROW(fcntl(this->fd, F_SETFL, flags | O_NONBLOCK));
		_ERR_RET_THROW(fcntl(this->fd, FD_CLOEXEC));
	}
}

Epoll::~Epoll() {
	if (this->fd != -1)
		close(this->fd);
}
bool Epoll::addCallback(int fd, EpollType eventType, Rc<Callback> callback) {
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

	struct epoll_event ev = {};
	ev.data.fd			  = fd;
	ev.events			 |= e.write.hasValue() ? (int)EPOLLOUT : 0;
	ev.events			 |= e.read.hasValue() ? (int)EPOLLIN : 0;
	ev.events			 |= e.hangup.hasValue() ? (int)(EPOLLHUP | EPOLLRDHUP) : 0;

	int op				  = new_ ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

	if (epoll_ctl(this->fd, op, fd, &ev) == -1) {
		int serrno = errno;
		(void)(serrno);	 // log macro
		LOG(err, "epoll_ctl: " << fd << ": " << strerror(serrno));
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

	struct epoll_event ev = {};
	ev.data.fd			  = fd;
	ev.events			 |= e.write.hasValue() ? (int)EPOLLOUT : 0;
	ev.events			 |= e.read.hasValue() ? (int)EPOLLIN : 0;
	ev.events			 |= e.hangup.hasValue() ? (int)(EPOLLHUP | EPOLLRDHUP) : 0;

	int op				  = (ev.events == 0) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;

	if (epoll_ctl(this->fd, op, fd, &ev) == -1) {
		int serrno = errno;
		(void)(serrno);	 // log macro
		LOG(err, "epoll_ctl: " << fd << ": " << strerror(serrno));
		return false;
	}

	if (op == EPOLL_CTL_DEL)
		this->callbacks.erase(this->callbacks.find(fd));

	return true;
}

std::vector<Rc<Callback> > Epoll::fetchCallbacks() {
	std::vector<Rc<Callback> > out;
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

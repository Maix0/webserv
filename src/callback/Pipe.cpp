/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pipe.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:02:42 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/22 18:54:04 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/State.hpp"
#include "app/fs/CgiPipe.hpp"
#include "app/http/Response.hpp"
#include "interface/AsFd.hpp"
#include "interface/Callback.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/EpollType.hpp"

void PipeCgi::CallbackHangup::call(Epoll& epoll, Rc<Callback> self) {
	(void)(self);
	epoll.removeCallback(this->asFd(), HANGUP);
	epoll.removeCallback(this->asFd(), READ);
	this->setFinished();

	while (true) {
		CgiList cgis;

		CgiList::iterator it = cgis.begin();
		for (it = cgis.begin(); it != cgis.end() && (*it)->asFd() != this->asFd(); it++)
			;
		if (it != cgis.end()) {
			cgis.erase(it);
		} else {
			break;
		}
	}
}

void PipeCgi::CallbackRead::call(Epoll& epoll, Rc<Callback> self) {
	if (this->res->isFinished())
		return;

	epoll.addCallback(self->getFd(), self->getTy(), self);
	char	buffer[1024];
	ssize_t out;

	if ((out = read(self->getFd(), buffer, sizeof(buffer))) < 0) {
		int serr = errno;
		(void)(serr);
		LOG(warn, "Failed to read on pipe for CGI: " << strerror(serr));
	}

	for (ssize_t i = 0; i < out; i++)
		this->output->push_back(buffer[i]);
}

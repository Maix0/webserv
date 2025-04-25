/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pipe.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:02:42 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/25 18:12:36 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/State.hpp"
#include "app/fs/CgiPipe.hpp"
#include "app/http/Response.hpp"
#include "interface/AsFd.hpp"
#include "interface/Callback.hpp"
#include "lib/PassthruDeque.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/EpollType.hpp"

static void remove_self(int selfFd) {
	while (true) {
		CgiList& cgis		 = State::getInstance().getCgis();

		CgiList::iterator it = cgis.begin();
		for (it = cgis.begin(); it != cgis.end() && (*it)->getPipeFd() != selfFd; it++)
			;
		if (it != cgis.end()) {
			cgis.erase(it);
		} else {
			break;
		}
	}
}

void PipeCgi::CallbackHangup::call(Epoll& epoll, Rc<Callback> self) {
	(void)(self);
	epoll.removeCallback(this->asFd(), HANGUP);
	epoll.removeCallback(this->asFd(), READ);
	this->setFinished();
	Option<Rc<Connection> > a;
	this->parent->updateTime();
	remove_self(this->asFd());
}

void PipeCgi::CallbackRead::call(Epoll& epoll, Rc<Callback> self) {
	this->parent->updateTime();

	epoll.addCallback(self->getFd(), self->getTy(), self);
	char	buffer[1024];
	ssize_t out;

	LOG(debug, "read for pipecgi");
	if ((out = read(self->getFd(), buffer, sizeof(buffer))) < 0) {
		int serr = errno;
		(void)(serr);
		LOG(warn, "Failed to read on pipe for CGI: " << strerror(serr));
	}
	for (ssize_t i = 0; i < out; i++)
		this->output->push_back(buffer[i]);
	LOG(info, "added char: '" << std::string(this->output->begin(), this->output->end()) << "' to "
							  << &*this->output);
}

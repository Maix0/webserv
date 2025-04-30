/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:02:42 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/30 22:58:36 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/State.hpp"
#include "app/http/CgiOutput.hpp"
#include "app/http/Response.hpp"
#include "interface/Callback.hpp"
#include "lib/PassthruDeque.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/EpollType.hpp"

template <>
void CgiOutput::PipeInstance::CHangup::call(Epoll& epoll, Rc<Callback> self) {
	(void)(self);
	epoll.removeCallback(this->asFd(), HANGUP);
	epoll.removeCallback(this->asFd(), READ);
	Option<Rc<CgiOutput> > p = this->parent.upgrade();
	if (p.hasValue()) {
		Option<Rc<Connection> > c = p.get()->conn.upgrade();
		if (c.hasValue())
			c.get()->updateTime();
		p.get()->setFinished();
	}
	while (true) {
		CgiList& cgis		 = State::getInstance().getCgis();

		CgiList::iterator it = cgis.begin();
		for (it = cgis.begin(); it != cgis.end() && (*it)->getPipeFd() != this->asFd(); it++)
			;
		if (it != cgis.end()) {
			cgis.erase(it);
		} else {
			break;
		}
	}
}

template <>
void CgiOutput::PipeInstance::CRead::call(Epoll& epoll, Rc<Callback> self) {
	Option<Rc<CgiOutput> > p = this->parent.upgrade();
	if (p.hasValue()) {
		Option<Rc<Connection> > c = p.get()->conn.upgrade();
		if (c.hasValue())
			c.get()->updateTime();
	} else {
		epoll.removeCallback(this->asFd(), HANGUP);
		epoll.removeCallback(this->asFd(), READ);
		while (true) {
			CgiList& cgis		 = State::getInstance().getCgis();

			CgiList::iterator it = cgis.begin();
			for (it = cgis.begin(); it != cgis.end() && (*it)->getPipeFd() != this->asFd(); it++)
				;
			if (it != cgis.end()) {
				cgis.erase(it);
			} else {
				break;
			}
		}
		return;
	}

	epoll.addCallback(self->getFd(), self->getTy(), self);
	char	buffer[1024];
	ssize_t out;

	if ((out = read(self->getFd(), buffer, sizeof(buffer))) < 0) {
		int serr = errno;
		(void)(serr);
		LOG(warn, "Failed to read on pipe for CGI: " << strerror(serr));
	}
	for (ssize_t i = 0; i < out; i++)
		p.get()->buffer.push_back(buffer[i]);
}

template class CgiOutput::PipeInstance::CB<READ>;
template class CgiOutput::PipeInstance::CB<HANGUP>;

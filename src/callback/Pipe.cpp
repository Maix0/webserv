/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pipe.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:02:42 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/22 11:21:10 by maiboyer         ###   ########.fr       */
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
	epoll.removeCallback(this->asFd(), HANGUP);
	epoll.removeCallback(this->asFd(), READ);

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
}

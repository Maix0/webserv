/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _Callback.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 16:26:45 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/25 22:46:00 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "interface/Callback.hpp"
#include "runtime/Epoll.hpp"

void ChainedCallback::call(Epoll& epoll, Rc<Callback> self) {
	/// if we are called, we propagate to inner Callback
	this->inner->call(epoll, this->inner);

	if (this->inner->isFinished()) {
		/// If the inner Callback is finished, we enqueue next Callback
		/// this will make us disapear (since we don't renqueue ourselves), dropping both refs
		/// to the callbacks
		/// It is now the responsability of `next` to manage its lifetime
		epoll.addCallback(this->next->getFd(), this->next->getTy(), this->next);
	} else {
		/// Otherwise we re-enqueue ourselves to overwrite the callback eventually set by out
		/// inner Callback
		/// We will keep managing the lifetime of `inner`
		epoll.addCallback(this->inner->getFd(), this->inner->getTy(), self);
	}
}

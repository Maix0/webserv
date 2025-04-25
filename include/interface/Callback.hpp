/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:59:36 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/25 18:29:58 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib/Rc.hpp"
#include "runtime/EpollType.hpp"

class Epoll;
// A callback function that handles an epoll event
class Callback {
	private:
		bool finished;

	public:
		Callback() : finished(false) {};
		virtual ~Callback() {};
		virtual void	  call(Epoll& epoll, Rc<Callback> self) = 0;
		virtual int		  getFd()								= 0;
		virtual EpollType getTy()								= 0;
		void			  setFinished();
		bool			  isFinished() const { return this->finished; };
};

class ChainedCallback : public Callback {
	private:
		Rc<Callback> inner;
		Rc<Callback> next;

	public:
		ChainedCallback(Rc<Callback> current, Rc<Callback> after) : inner(current), next(after) {};
		virtual ~ChainedCallback() {};

		virtual void call(Epoll& epoll, Rc<Callback> self);

		virtual int getFd() { return this->inner->getFd(); }

		virtual EpollType getTy() { return this->inner->getTy(); }
};

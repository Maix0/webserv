/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:59:36 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/13 17:12:55 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "app/EpollType.hpp"
#include "app/Option.hpp"
#include "app/Shared.hpp"

namespace app {
	class Epoll;
	// A callback function that handles an epoll event
	class Callback {
	private:
		bool finished;

	public:
		Callback() : finished(false) {};
		virtual ~Callback() {};
		virtual void	  call(Epoll& epoll, app::Shared<Callback> self) = 0;
		virtual int		  getFd()										 = 0;
		virtual EpollType getTy()										 = 0;
		void			  setFinished() { this->finished = true; };
		bool			  isFinished() const { return this->finished; };
	};

	class ChainedCallback : public Callback {
	private:
		Shared<Callback> inner;
		Shared<Callback> next;

	public:
		ChainedCallback(Shared<Callback> current, Shared<Callback> after)
			: inner(current), next(after) {};
		virtual ~ChainedCallback() {};

		virtual void	  call(Epoll& epoll, app::Shared<Callback> self);

		virtual int		  getFd() { return this->inner->getFd(); }

		virtual EpollType getTy() { return this->inner->getTy(); }
	};
}  // namespace app

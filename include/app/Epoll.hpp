/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:01:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/13 16:16:00 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/epoll.h>
#include <map>
#include <vector>
#include "app/Callback.hpp"
#include "app/Option.hpp"
#include "app/Shared.hpp"

namespace app {
	class Epoll {
	private:
		struct EpollCallback {
			Option<Shared<Callback> > read;
			Option<Shared<Callback> > write;
			Option<Shared<Callback> > hangup;
		};

		typedef std::map<int, EpollCallback> CallbackStorage;
		// no copy for you !
		Epoll(const Epoll&);
		Epoll&			operator=(const Epoll&);

		int				fd;
		CallbackStorage callbacks;

	public:
		static const int MAX_EVENTS	   = 5000;
		static const int EPOLL_TIMEOUT = -1;
		Epoll();
		~Epoll();

		/// @return true in success, false on error
		bool						   addCallback(int fd, EpollType ty, Shared<Callback> callback);
		/// @return true in success, false on error
		bool						   removeCallback(int fd, EpollType ty);

		std::vector<Shared<Callback> > fetchCallbacks();
	};

};	// namespace app

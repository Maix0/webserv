/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:01:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 14:50:17 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/epoll.h>
#include <cstddef>
#include <map>
#include <vector>
#include "app/Callback.hpp"
#include "app/Shared.hpp"
#include "app/Socket.hpp"

namespace app {
	struct EpollEvent {
		bool read, write;
	};

	class Epoll {
	private:
		// no copy for you !
		Epoll(const Epoll&);
		Epoll&							 operator=(const Epoll&);

		int								 fd;
		std::map<int, Shared<Callback> > callbacks;

	public:
		static const int MAX_EVENTS	   = 5000;
		static const int EPOLL_TIMEOUT = 1000;
		Epoll();
		~Epoll();

		/// @return true in success, false on error
		bool addCallback(int fd, int eventType, Shared<Callback> callback);
		/// @return true in success, false on error
		bool removeCallback(int fd);

		const std::map<int, Shared<Callback> >& getCallbacks() const { return (this->callbacks); }

		std::vector<std::pair<EpollEvent, Shared<Callback> > > fetchCallbacks();
	};

};	// namespace app

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shutdown.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 15:00:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 15:02:24 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Context.hpp"
#include "app/Epoll.hpp"
#include "app/Logger.hpp"
#include "app/Socket.hpp"

#include <fcntl.h>
#include <sys/socket.h>
namespace app {
	void ShutdownCallback::call(Epoll& epoll, EpollEvent event) {
		(void)(epoll);
		(void)(event);
		LOG(info, "Shutdown has been requested");
		*this->shutdown = true;
	};
};	// namespace app

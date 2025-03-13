/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shutdown.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 15:00:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/13 16:24:27 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Context.hpp"
#include "app/Epoll.hpp"
#include "app/Logger.hpp"
#include "app/Shared.hpp"
#include "app/Socket.hpp"

#include <fcntl.h>
#include <sys/socket.h>
namespace app {
	void ShutdownCallback::call(Epoll& epoll, app::Shared<Callback> self) {
		(void)(epoll);
		(void)(self);
		LOG(info, "Shutdown has been requested");
		*this->shutdown = true;
		self->setFinished();
	};
};	// namespace app

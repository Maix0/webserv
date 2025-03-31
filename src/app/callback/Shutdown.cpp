/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shutdown.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 15:00:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/25 22:46:49 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Epoll.hpp"
#include "app/Logger.hpp"
#include "app/Shared.hpp"
#include "app/Socket.hpp"

#include <fcntl.h>
#include <sys/socket.h>

void ShutdownCallback::call(Epoll& epoll, Shared<Callback> self) {
	(void)(epoll);
	(void)(self);
	LOG(info, "Shutdown has been requested");
	*this->shutdown = true;
	self->setFinished();
};

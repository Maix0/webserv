/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shutdown.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 15:00:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/26 21:45:37 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "runtime/Epoll.hpp"
#include "runtime/Logger.hpp"
#include "lib/Rc.hpp"
#include "app/net/Socket.hpp"

#include <fcntl.h>
#include <sys/socket.h>

void ShutdownCallback::call(Epoll& epoll, Rc<Callback> self) {
	(void)(epoll);
	(void)(self);
	LOG(info, "Shutdown has been requested");
	*this->shutdown = true;
};

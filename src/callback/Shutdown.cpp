/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shutdown.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 15:00:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/02 10:45:55 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/State.hpp"
#include "app/net/Socket.hpp"
#include "lib/Rc.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/Logger.hpp"

void ShutdownCallback::call(Epoll& epoll, Rc<Callback> self) {
	(void)(epoll);
	(void)(self);
	LOG(info, "Shutdown has been requested");
	do_shutdown = true;
}

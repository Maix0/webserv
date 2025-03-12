/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 14:28:15 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 15:01:08 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Socket.hpp"
#include "app/Context.hpp"
#include "app/Epoll.hpp"
#include "app/Logger.hpp"

#include <fcntl.h>
#include <sys/socket.h>
namespace app {
	void SocketCallback::call(Epoll& epoll, EpollEvent event) {
		(void)(epoll);
		(void)(event);
		LOG(debug, "Called for socket: " << this->socketfd->asFd());

		struct sockaddr_storage addr;
		socklen_t				addr_size = 0;

		int res = accept(this->socketfd->asFd(), (struct sockaddr*)&addr, &addr_size);
		if (res == -1) {
			LOG(err, "Failed to accept connection for socket " << this->socketfd->asFd());
			return;
		}
		fcntl(res, FD_CLOEXEC);
		LOG(trace, "Sending hello to new connection :)");
		(void)!write(res, "hello\n", 6);
		fcntl(res, F_SETFL, O_NONBLOCK);
		// Context& ctx = Context::getInstance();

		close(res);
	};
};	// namespace app

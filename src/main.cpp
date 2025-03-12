/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 15:01:24 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include <typeinfo>

#include "app/Callback.hpp"
#include "app/Context.hpp"
#include "app/Epoll.hpp"
#include "app/Logger.hpp"
#include "app/Shared.hpp"
#include "app/Socket.hpp"
#include "config/Config.hpp"
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

int wrapped_main(char* argv0, int argc, char* argv[], char* envp[]) {
	(void)(argc);
	(void)(argv);
	(void)(envp);

	if (argc != 1) {
		std::cout << "usage: " << (argv0 ? argv0 : "webserv") << " <config_file>" << std::endl;
		return 1;
	}

	app::Context&	ctx	   = app::Context::getInstance();

	toml::Value		val	   = toml::Parser::parseFile(argv[0]);

	config::Config& config = (ctx.getConfig() = config::Config::fromTomlValue(val));

	config::checkConfig(config, envp);
	ctx.openAllSockets();

	app::SocketList s = ctx.getSockets();
	app::Epoll		epoll;

	for (app::SocketList::iterator iit = s.begin(); iit != s.end(); iit++) {
		for (std::vector<app::Shared<app::Socket> >::iterator sit = iit->second.begin();
			 sit != iit->second.end(); sit++) {
			app::Shared<app::Socket>		 sock	 = *sit;
			app::Shared<app::SocketCallback> sock_cb = new app::SocketCallback(sock);
			epoll.addCallback(sock->asFd(), EPOLLIN, sock_cb.cast<app::Callback>());
		}
	}
	app::Shared<bool> shutdown = new bool(false);

	{
		app::Shared<app::Socket> shutdown_socket = new app::Socket(app::Ip(0), app::Port(0));
		app::Shared<app::ShutdownCallback> shutdown_cb =
			new app::ShutdownCallback(shutdown_socket, shutdown);
		epoll.addCallback(shutdown_socket->asFd(), EPOLLIN, shutdown_cb.cast<app::Callback>());
	}
	while (!*shutdown) {
		std::vector<std::pair<app::EpollEvent, app::Shared<app::Callback> > > callbacks =
			epoll.fetchCallbacks();
		for (std::vector<std::pair<app::EpollEvent, app::Shared<app::Callback> > >::iterator it =
				 callbacks.begin();
			 it != callbacks.end(); it++) {
			app::EpollEvent			   event = it->first;
			app::Shared<app::Callback> cb	 = it->second;
			cb->call(epoll, event);
		}
		sleep(1);
	};

	return 0;
}

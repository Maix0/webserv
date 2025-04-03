/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/02 15:02:43 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include <typeinfo>

#include "interface/Callback.hpp"
#include "app/State.hpp"
#include "app/fs/Directory.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/Logger.hpp"
#include "app/http/Routing.hpp"
#include "lib/Rc.hpp"
#include "app/net/Socket.hpp"
#include "config/Config.hpp"
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

using std::string;
using std::vector;

Rc<bool> do_shutdown = new bool(false);

static void install_ctrlc_handler(void);

int wrapped_main(char* argv0, int argc, char* argv[], char* envp[]) {
	(void)(argv0);
	(void)(argc);
	(void)(envp);

	if (argc != 1) {
		std::cout << "usage: " << (argv0 ? argv0 : "webserv") << " <config_file>" << std::endl;
		return 1;
	}

	State& ctx	   = State::getInstance();

	toml::Value val		   = toml::Parser::parseFile(argv[0]);

	config::Config& config = (ctx.getConfig() = config::Config::fromTomlValue(val));

	config::checkConfig(config, envp);
	ctx.openAllSockets();

	SocketList s = ctx.getSockets();
	Epoll		epoll;

	for (SocketList::iterator iit = s.begin(); iit != s.end(); iit++) {
		for (vector<Rc<Socket> >::iterator sit = iit->second.begin();
			 sit != iit->second.end(); sit++) {
			Rc<Socket>		 sock	 = *sit;
			Rc<SocketCallback> sock_cb = new SocketCallback(sock);
			epoll.addCallback(sock->asFd(), READ, sock_cb.cast<Callback>());
		}
	}

	if (config.shutdown_port.hasValue()) {
		Rc<Socket> shutdown_socket =
			new Socket(Ip(0), config.shutdown_port.get());
		ctx.getShutdown() = shutdown_socket;
		LOG(info, "Created shutdown socket on port: " << shutdown_socket->getBoundPort());
		Rc<ShutdownCallback> shutdown_cb =
			new ShutdownCallback(shutdown_socket, do_shutdown);
		epoll.addCallback(shutdown_socket->asFd(), READ, shutdown_cb.cast<Callback>());
	}
	install_ctrlc_handler();
	while (!*do_shutdown) {
		vector<Rc<Callback> > callbacks = epoll.fetchCallbacks();
		for (vector<Rc<Callback> >::iterator it = callbacks.begin();
			 it != callbacks.end(); it++) {
			Rc<Callback> cb = *it;
			cb->call(epoll, cb);
		}
	};
	LOG(info, "shutting down now...");
	return 0;
}
#define BONUS
#ifndef BONUS
static void install_ctrlc_handler(void) {};
#else
#	include <csignal>

static void _ctrlc_handler(int sig) {
	(void)(sig);
	LOG(warn, "Ctrl+C: Shutting down");
	*do_shutdown = true;
}

static void install_ctrlc_handler(void) {
	signal(SIGINT, _ctrlc_handler);
};

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/14 00:22:23 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

#include "app/State.hpp"
#include "app/fs/Directory.hpp"
#include "app/http/Routing.hpp"
#include "app/net/Socket.hpp"
#include "config/Config.hpp"
#include "interface/Callback.hpp"
#include "lib/Rc.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/Logger.hpp"
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

	State& ctx			   = State::getInstance();

	toml::Value val		   = toml::Parser::parseFile(argv[0]);

	config::Config& config = (ctx.getConfig() = config::Config::fromTomlValue(val));

	config::checkConfig(config, envp);
	ctx.openAllSockets();

	SocketList s = ctx.getSockets();
	Epoll	   epoll;

	for (SocketList::iterator iit = s.begin(); iit != s.end(); iit++) {
		for (vector<Rc<Socket> >::iterator sit = iit->second.begin(); sit != iit->second.end();
			 sit++) {
			Rc<Socket>		   sock	   = *sit;
			Rc<SocketCallback> sock_cb = new SocketCallback(sock);
			{
				int fd = -1;
				_ERR_RET_THROW(fd = open("/tmp/socket_webserv", O_CREAT | O_TRUNC | O_RDWR, 0777));
				dprintf(fd, "%i\n", sock->getBoundPort().inner);
				close(fd);
			}
			epoll.addCallback(sock->asFd(), READ, sock_cb.cast<Callback>());
		}
	}

	if (config.shutdown_port.hasValue()) {
		Rc<Socket> shutdown_socket = new Socket(Ip(0), config.shutdown_port.get());
		ctx.getShutdown()		   = shutdown_socket;
		LOG(info, "Created shutdown socket on port: " << shutdown_socket->getBoundPort());
		Rc<ShutdownCallback> shutdown_cb = new ShutdownCallback(shutdown_socket, do_shutdown);
		epoll.addCallback(shutdown_socket->asFd(), READ, shutdown_cb.cast<Callback>());
	}
	install_ctrlc_handler();
	vector<size_t>			 to_indexes;
	size_t					 idx = 0;
	ConnectionList::iterator conn;
	while (!*do_shutdown) {
		vector<Rc<Callback> > callbacks = epoll.fetchCallbacks();
		for (vector<Rc<Callback> >::iterator it = callbacks.begin(); it != callbacks.end(); it++) {
			Rc<Callback> cb = *it;
			cb->call(epoll, cb);
		}
		ConnectionList& connections = ctx.getConnections();
		to_indexes.clear();
		for (conn = connections.begin(), idx = 0; conn != connections.end(); conn++, idx++)
			if ((*conn)->timeout())
				to_indexes.push_back(idx);
		for (vector<size_t>::reverse_iterator it = to_indexes.rbegin(); it != to_indexes.rend();
			 it++) {
			ConnectionList::iterator c = connections.begin() + *it;
			epoll.removeCallback((*c)->asFd(), WRITE);
			epoll.removeCallback((*c)->asFd(), READ);
			epoll.removeCallback((*c)->asFd(), HANGUP);
			LOG(trace, "Keep alive timeout for " << (*c)->asFd());
			connections.erase(c);
		}
	};
	LOG(info, "shutting down now...");
	return 0;
}

#include <csignal>

static void _ctrlc_handler(int sig) {
	(void)(sig);
	LOG(warn, "Ctrl+C: Shutting down");
	*do_shutdown = true;
}

static void install_ctrlc_handler(void) {
	signal(SIGINT, _ctrlc_handler);
};

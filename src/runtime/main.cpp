/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/10 14:13:41 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

#include "app/State.hpp"
#include "app/fs/Directory.hpp"
#include "app/http/Response.hpp"
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

bool do_shutdown = false;

static void install_signals(void);
static void update_childs();

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
	ctx.setEnv(envp);

	SocketList s = ctx.getSockets();
	Epoll	   epoll;

	for (SocketList::iterator iit = s.begin(); iit != s.end(); iit++) {
		for (vector<Rc<Socket> >::iterator sit = iit->second.begin(); sit != iit->second.end();
			 sit++) {
			Rc<Socket> sock = *sit;
#ifdef ENABLE_SOCKET_PORT
			int fd = -1;
			_ERR_RET_THROW(fd = open("/tmp/socket_webserv", O_CREAT | O_TRUNC | O_RDWR, 0777));
			dprintf(fd, "%i\n", sock->getBoundPort().inner);
			close(fd);
#endif
			Rc<SocketCallback> sock_cb =
				Rc<SocketCallback>(Functor1<SocketCallback, Rc<Socket>&>(sock), RCFUNCTOR);
			epoll.addCallback(sock->asFd(), READ, sock_cb.cast<Callback>());
		}
	}

	if (config.shutdown_port.hasValue()) {
		Rc<Socket> shutdown_socket =
			Rc<Socket>(Functor2<Socket, Ip, Port>(Ip(0), config.shutdown_port.get()), RCFUNCTOR);
		ctx.getShutdown() = shutdown_socket;
		LOG(info, "Created shutdown socket on port: " << shutdown_socket->getBoundPort());
		Rc<ShutdownCallback> shutdown_cb = Rc<ShutdownCallback>(
			Functor1<ShutdownCallback, Rc<Socket>&>(shutdown_socket), RCFUNCTOR);
		epoll.addCallback(shutdown_socket->asFd(), READ, shutdown_cb.cast<Callback>());
	}
	// if (false)
	install_signals();
	vector<size_t>			 to_indexes;
	size_t					 idx = 0;
	ConnectionList::iterator conn;
	while (!do_shutdown) {
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
		update_childs();
		vector<pid_t>				  pids;
		IndexMap<pid_t, ChildStatus>& cstatus = ctx.getChildStatus();
		CgiList&					  cgis	  = ctx.getCgis();
		for (IndexMap<pid_t, ChildStatus>::iterator it = cstatus.begin(); it != cstatus.end();
			 it++) {
			if (it->second.is_finished) {
				pids.push_back(it->first);
			}
		}
		for (vector<pid_t>::iterator it = pids.begin(); it != pids.end(); it++) {
			cstatus.erase(cstatus.find_key(*it));
			CgiList::iterator cit;
			for (cit = cgis.begin(); cit != cgis.end() && (*cit)->getPid() != *it; cit++)
				;
			if (cit != cgis.end()) {
				(*cit)->setFinished();
			}
		}
	};
	LOG(info, "shutting down now...");
	return 0;
}

#include <sys/wait.h>
#include <csignal>

static void _sigint_handler(int sig) {
	(void)(sig);
	LOG(warn, "Ctrl+C: Shutting down");
	do_shutdown = true;
}

static void update_childs() {
	int							  child_pid = -1;
	int							  status;
	IndexMap<pid_t, ChildStatus>& cstatus = State::getInstance().getChildStatus();

	while ((child_pid = waitpid(-1, &status, WNOHANG)) != -1) {
		if (child_pid == 0)
			continue;
		int exit_code = -1;
		if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exit_code = WSTOPSIG(status) + 128;

		if (exit_code != -1) {
			if (cstatus.count(child_pid) != 0) {
				LOG(debug, "child " << child_pid << " finished with exit code " << exit_code);
				cstatus.at(child_pid).exit_code	  = exit_code;
				cstatus.at(child_pid).is_finished = true;
			} else {
				LOG(warn, "unknown child pid " << child_pid);
			}
		}
	}
}
static void install_signals(void) {
	signal(SIGINT, _sigint_handler);
}

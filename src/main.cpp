/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 18:35:13 by maiboyer         ###   ########.fr       */
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

app::Shared<bool> app::do_shutdown = new bool(false);

static void		  install_ctrlc_handler(void);

int				  wrapped_main(char* argv0, int argc, char* argv[], char* envp[]) {
	  (void)(argc);
	  (void)(argv);
	  (void)(envp);

	  if (argc != 1) {
		  std::cout << "usage: " << (argv0 ? argv0 : "webserv") << " <config_file>" << std::endl;
		  return 1;
	  }

	  app::Context&	  ctx	 = app::Context::getInstance();

	  toml::Value	  val	 = toml::Parser::parseFile(argv[0]);

	  config::Config& config = (ctx.getConfig() = config::Config::fromTomlValue(val));

	  config::checkConfig(config, envp);
	  ctx.openAllSockets();

	  app::SocketList s = ctx.getSockets();
	  app::Epoll	  epoll;

	  for (app::SocketList::iterator iit = s.begin(); iit != s.end(); iit++) {
		  for (std::vector<app::Shared<app::Socket> >::iterator sit = iit->second.begin();
			   sit != iit->second.end(); sit++) {
			  app::Shared<app::Socket>		   sock	   = *sit;
			  app::Shared<app::SocketCallback> sock_cb = new app::SocketCallback(sock);
			  epoll.addCallback(sock->asFd(), app::Epoll::READ, sock_cb.cast<app::Callback>());
		  }
	  }

	  if (config.shutdown_port.hasValue()) {
		  app::Shared<app::Socket> shutdown_socket =
			  new app::Socket(app::Ip(0), config.shutdown_port.get());
		  LOG(info, "Created shutdown socket on port: " << shutdown_socket->getBoundPort());
		  app::Shared<app::ShutdownCallback> shutdown_cb =
			  new app::ShutdownCallback(shutdown_socket, app::do_shutdown);
		  epoll.addCallback(shutdown_socket->asFd(), app::Epoll::READ,
										shutdown_cb.cast<app::Callback>());
	  }
	  install_ctrlc_handler();
	  while (!*app::do_shutdown) {
		  std::vector<app::Shared<app::Callback> > callbacks = epoll.fetchCallbacks();
		  for (std::vector<app::Shared<app::Callback> >::iterator it = callbacks.begin();
			   it != callbacks.end(); it++) {
			  app::Shared<app::Callback> cb = *it;
			  cb->call(epoll, cb);
		  }
	  };
	  LOG(info, "shutting down now...");
	  return 0;
}

#ifndef BONUS
static void install_ctrlc_handler(void) {};
#else
#	include <csignal>

static void _ctrlc_handler(int sig) {
	(void)(sig);
	LOG(warn, "Ctrl+C: Shutting down");
	*app::do_shutdown = true;
}

static void install_ctrlc_handler(void) {
	signal(SIGINT, _ctrlc_handler);
};

#endif

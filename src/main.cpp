/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:59:06 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include "app/Logger.hpp"

#include "app/Context.hpp"
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

	toml::Value		val	   = toml::Parser::parseFile(argv[1]);

	config::Config& config = (ctx.getConfig() = config::Config::fromTomlValue(val));

	for (std::map<std::string, config::Listener>::iterator it = config.listener.begin();
		 it != config.listener.end(); it++) {
		ctx.getSockets().insert(std::make_pair(it->first, std::vector<app::Socket>()));
		std::vector<app::Socket>& socklist = ctx.getSockets().at(it->first);
		LOG(debug, "opening socket for " << it->first);
		try {
			app::Socket sock = app::Socket(it->second.host, it->second.port.at(0));
			socklist.push_back(sock);
		} catch (const std::exception& e) {
			LOG(err, "error for listener " << it->first << " : " << e.what());
		}
	}
	return 0;
}

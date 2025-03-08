/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/08 17:53:48 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include <sstream>
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

	toml::Value		val	   = toml::Parser::parseFile(argv[0]);

	config::Config& config = (ctx.getConfig() = config::Config::fromTomlValue(val));

	config::checkConfig(config);
	ctx.openAllSockets();
	
	return 0;
}

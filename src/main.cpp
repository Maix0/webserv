/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/02 22:26:59 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include "app/Logger.hpp"

#include "app/Context.hpp"
#include "config/Config.hpp"
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

int wrapped_main(int argc, char* argv[], char* envp[]) {
	(void)(argc);
	(void)(argv);
	(void)(envp);

	if (argc != 2) {
		std::cout << "usage: " << (argv[0] ? argv[0] : "webserv") << " <config_file>" << std::endl;
		return 1;
	}

	Context&  ctx	 = Context::getInstance();
	TomlValue val	 = TomlParser::parseFile(argv[1]);
	LOG(debug, val);
	Config	  config = Config::fromTomlValue(val);
	LOG(info, config);
	(void)(ctx);
	return 0;
}

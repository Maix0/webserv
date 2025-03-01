/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/01 12:34:44 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include "config/Config.hpp"
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

int _main(int argc, char* argv[], char* envp[]) {
	(void)(argc);
	(void)(argv);
	(void)(envp);

	TomlValue val	 = TomlParser::parseFile("./config.toml");
	Config	  config = Config::fromTomlValue(val);
	std::cout << config << std::endl;
	return 0;
}

int main(int argc, char* argv[], char* envp[]) {
	try {
		return _main(argc, argv, envp);
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return 1;
}

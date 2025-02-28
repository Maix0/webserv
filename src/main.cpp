/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/28 14:32:24 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

int main(void) {
	try {
		TomlValue val = TomlParser::parseFile("./config.toml");
		std::cout << val << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}

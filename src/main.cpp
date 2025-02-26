/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/26 10:11:56 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

int main(void) {
	TomlValue val = TomlValue::newBool();
	std::cout << val << std::endl;

	TomlValue val2 = TomlValue::newList();
	std::cout << val2 << std::endl;

	val2.getList().push_back(val);
	val2.getList().push_back(TomlValue("Banane"));
	val2.getList().push_back(TomlValue::newNull());
	val2.getList().push_back(TomlValue(150));
	val2.getList().push_back(TomlValue(val2));

	std::cout << val2 << std::endl;
	val2 = TomlValue("SOMETHING AMAZING !");

	std::cout << val2 << std::endl;
	val2									= TomlValue::newTable();
	val2.getTable()["Banana"]				= TomlValue("miam\t\t\t");
	val2.getTable()["something"]			= TomlValue(9);
	val2.getTable()["saaaa"]				= TomlValue(val);
	val2.getTable()["config.maix.me\u20AC"] = TomlValue::newTable();

	std::cout << val2 << std::endl;

	val2 = TomlValue(7);

	std::cout << val2 << std::endl;

	try {
		val2 = TomlParser::parseFile("./Makefile");
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/23 23:37:22 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
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
	val2							  = TomlValue::newTable();
	val2.getTable()["Banana"]		  = TomlValue("miam");
	val2.getTable()["something"]	  = TomlValue(9);
	val2.getTable()["saaaa"]		  = TomlValue(val);
	val2.getTable()["config.maix.me"] = TomlValue::newTable();
	
	std::cout << val2 << std::endl;

	return 0;
}

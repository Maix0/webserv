/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 00:07:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/24 13:10:13 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <string>
#include "toml/TomlValue.hpp"

void _print_handle_utf8(std::stringstream& buf, TomlValue::TomlString::const_iterator& it, TomlValue::TomlString::const_iterator end);

void t(const TomlValue::TomlString& chr) {
	std::stringstream					  ss;

	TomlValue::TomlString::const_iterator it = chr.begin();
	_print_handle_utf8(ss, it, chr.end());
	std::cout << "STR: \"" << chr << "\"; MINE: \"" << ss.str() << "\"" << std::endl;
}

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
	val2.getTable()["Banana"]		  = TomlValue("miam\t\t\t");
	val2.getTable()["something"]	  = TomlValue(9);
	val2.getTable()["saaaa"]		  = TomlValue(val);
	val2.getTable()["config.maix.me\u20AC"] = TomlValue::newTable();

	std::cout << val2 << std::endl;
	return 0;
}

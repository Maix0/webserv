/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ConfigHelper.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:55:33 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/28 21:49:45 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include "toml/TomlValue.hpp"

template <typename T>
std::map<std::string, T> _handle_map(const TomlValue& val, T (*func)(const TomlValue&)) {
	if (!val.isTable())
		throw std::runtime_error("Value isn't a table");

	const TomlTable&		 table = val.getTable();
	std::map<std::string, T> out;

	for (TomlTable::const_iterator it = table.begin(); it != table.end(); it++) {
		try {
			out[it->first] = (func)(it->second);
		} catch (const std::exception& e) {
			throw std::runtime_error(it->first + " " + e.what());
		}
	}
	return out;
}

#define WRAP_ERROR(TYPE, PREFIX, CODE)                        \
	do {                                                      \
		try {                                                 \
			CODE;                                             \
		} catch (const std::exception& e) {                   \
			throw TYPE(std::string(PREFIX) + " " + e.what()); \
		}                                                     \
	} while (0)

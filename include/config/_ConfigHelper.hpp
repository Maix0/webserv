/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _ConfigHelper.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:55:33 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/18 23:35:36 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include "lib/IndexMap.hpp"
#include "toml/Value.hpp"
namespace config {
	template <typename T>
	std::map<std::string, T> _handle_map(const toml::Value& val, T (*func)(const toml::Value&)) {
		if (!val.isTable())
			throw std::runtime_error("Value isn't a table");

		const toml::Table&		 table = val.getTable();
		std::map<std::string, T> out;

		for (toml::Table::const_iterator it = table.begin(); it != table.end(); it++) {
			try {
				out[it->first] = (func)(it->second);
			} catch (const std::exception& e) {
				throw std::runtime_error(std::string("\"") + it->first + "\" " + e.what());
			}
		}
		return out;
	}

	template <typename T>
	IndexMap<std::string, T> _handle_map_and_set_name(const toml::Value& val,
													  T (*func)(const toml::Value&)) {
		if (!val.isTable())
			throw std::runtime_error("Value isn't a table");

		const toml::Table&		 table = val.getTable();
		IndexMap<std::string, T> out;

		for (toml::Table::const_iterator it = table.begin(); it != table.end(); it++) {
			try {
				out[it->first]		= (func)(it->second);
				out[it->first].name = it->first;
			} catch (const std::exception& e) {
				throw std::runtime_error(std::string("\"") + it->first + "\" " + e.what());
			}
		}
		return out;
	}

	static inline std::string _toml_get_string(const toml::Value& val) {
		return (val.getString());
	}
}  // namespace config

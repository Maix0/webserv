/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:40:24 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <exception>
#include <iostream>
#include <set>
#include <stdexcept>

#include "app/Logger.hpp"
#include "app/Socket.hpp"
#include "config/Config.hpp"
#include "config/_ConfigHelper.hpp"
#include "toml/Value.hpp"

namespace config {
Server Server::fromTomlValue(const toml::Value& toml) {
	const toml::Table&	  table = toml.getTable();
	Server				  out;
	std::set<std::string> seen;

	for (toml::Table::const_iterator it = table.begin(); it != table.end(); it++) {
		try {
			seen.insert(it->first);
			if (it->first == "routes") {
				out.routes = _handle_map(it->second, Route::fromTomlValue);
			} else if (it->first == "root")
				out.root = it->second.getString();
			else if (it->first == "server_name") {
				if (it->second.isNull())
					out.servername = Option<std::string>::None();
				else
					out.servername = Option<std::string>::Some(it->second.getString());
			} else if (it->first == "errors") {
				out.errors = _handle_map(it->second, _toml_get_string);
			} else if (it->first == "listener") {
				out.listener = it->second.getString();
			} else
				throw std::runtime_error(std::string("unknown key"));
		} catch (const std::exception& e) {
			throw ServerParseError(std::string("\"") + it->first + "\" " + e.what());
		}
	}

	if (seen.count("root") == 0)
		throw ServerParseError("missing key \"root\"");
	if (seen.count("listener") == 0)
		throw ServerParseError("missing key \"listener\"");

	return out;
}
}  // namespace config

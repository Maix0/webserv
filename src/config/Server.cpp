/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/28 21:48:24 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <iostream>
#include <set>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/_ConfigHelper.hpp"
#include "toml/TomlValue.hpp"

static std::string _toml_get_string(const TomlValue& val) {
	return (val.getString());
}

Server Server::fromTomlValue(const TomlValue& toml) {
	const TomlTable&	  table = toml.getTable();
	Server				  out;
	std::set<std::string> seen;

	for (TomlTable::const_iterator it = table.begin(); it != table.end(); it++) {
		try {
			seen.insert(it->first);
			if (it->first == "routes") {
				out.routes = _handle_map(it->second, Route::fromTomlValue);
			} else if (it->first == "root")
				out.root = it->second.getString();
			else if (it->first == "host") {
				if (it->second.isNull())
					out.host = Option<std::string>::None();
				else
					out.host = Option<std::string>::Some(it->second.getString());
			} else if (it->first == "errors") {
				out.errors = _handle_map(it->second, _toml_get_string);
			} else if (it->first == "port") {
				if (it->second.isInt()) {
					TomlNumber p = it->second.getInt();
					if (p < 0 || p > 65565)
						throw std::runtime_error("Port is not out of range (0-65565)");
					unsigned short up = static_cast<unsigned short>(p);
					out.ports.push_back(up);
				} else {
					const TomlList& l = it->second.getList();
					for (TomlList::const_iterator it = l.begin(); it != l.end(); it++) {
						TomlNumber p = it->getInt();
						if (p < 0 || p > 65565)
							throw std::runtime_error("port is not out of range (0-65565)");
						unsigned short up = static_cast<unsigned short>(p);
						if (std::find(out.ports.begin(), out.ports.end(), up) != out.ports.end())
							std::cerr << "Duplicate port in server (skipping...): " << up
									  << std::endl;
						else
							out.ports.push_back(up);
					}
				}
			} else
				throw std::runtime_error(std::string("unknown key"));
		} catch (const std::exception& e) {
			throw ServerParseError(it->first + " " + e.what());
		}
	}
	if (seen.count("root") == 0)
		throw ServerParseError("missing key \"root\"");
	if (seen.count("port") == 0)
		throw ServerParseError("missing key \"port\"");

	return out;
}

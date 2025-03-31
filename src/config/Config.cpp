/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/14 16:40:36 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <sstream>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/_ConfigHelper.hpp"
#include "toml/Value.hpp"

namespace config {
	Config Config::fromTomlValue(const toml::Value& toml) {
		const toml::Table& table = toml.getTable();
		Config			   out;
		out.shutdown_port = Option<Port>::None();
		for (toml::Table::const_iterator it = table.begin(); it != table.end(); it++) {
			try {
				if (it->first == "cgi")
					out.cgi = _handle_map(it->second, Cgi::fromTomlValue);
				else if (it->first == "server")
					out.server = _handle_map_and_set_name(it->second, Server::fromTomlValue);
				else if (it->first == "shutdown") {
					if (it->second.isBool()) {
						if (it->second.getBool())
							out.shutdown_port = Option<Port>::Some(0);
						else
							out.shutdown_port = Option<Port>::None();
					} else if (it->second.isNull())
						out.shutdown_port = Option<Port>::None();
					else {
						toml::Number raw = it->second.getInt();
						if (raw < 0 || raw > 65565)
							throw std::runtime_error("invalid range (0-65565)");
						out.shutdown_port = Port(raw);
					}
				} else
					throw std::runtime_error("unknown key");
			} catch (const std::exception& e) {
				throw ConfigParseError(std::string("\"") + it->first + "\" " + e.what());
			}
		}
		return out;
	}
}  // namespace config

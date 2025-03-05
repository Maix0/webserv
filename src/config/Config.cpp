/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/05 16:59:02 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <sstream>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/_ConfigHelper.hpp"
#include "toml/TomlValue.hpp"

Config Config::fromTomlValue(const TomlValue& toml) {
	const TomlTable& table = toml.getTable();
	Config			 out;
	for (TomlTable::const_iterator it = table.begin(); it != table.end(); it++) {
		try {
			if (it->first == "cgi")
				out.cgi = _handle_map(it->second, Cgi::fromTomlValue);
			else if (it->first == "server")
				out.server = _handle_map(it->second, Server::fromTomlValue);
			else if (it->first == "listener")
				out.listener = _handle_map(it->second, Listener::fromTomlValue);
			else
				throw std::runtime_error("unknown key");
		} catch (const std::exception& e) {
			throw ConfigParseError(std::string("\"") + it->first + "\" " + e.what());
		}
	}

	for (std::map<std::string, Server>::const_iterator it = out.server.begin();
		 it != out.server.end(); it++) {
		if (out.listener.count(it->second.listener) == 0) {
			std::stringstream ss;
			ss << "server '" << it->first << "' tries to use unknown listener '"
			   << it->second.listener << "'";
			throw ConfigParseError(ss.str());
		}
	}
	return out;
}

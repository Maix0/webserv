/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/07 21:15:47 by maiboyer         ###   ########.fr       */
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
		for (toml::Table::const_iterator it = table.begin(); it != table.end(); it++) {
			try {
				if (it->first == "cgi")
					out.cgi = _handle_map(it->second, Cgi::fromTomlValue);
				else if (it->first == "server")
					out.server = _handle_map(it->second, Server::fromTomlValue);
				else
					throw std::runtime_error("unknown key");
			} catch (const std::exception& e) {
				throw ConfigParseError(std::string("\"") + it->first + "\" " + e.what());
			}
		}
		return out;
	}
}  // namespace config

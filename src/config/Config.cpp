/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/28 16:00:38 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdexcept>

#include "config/Config.hpp"
#include "config/_ConfigHelper.hpp"
#include "toml/TomlValue.hpp"

Config Config::fromTomlValue(const TomlValue& toml) {
	const TomlTable& table = toml.getTable();
	Config			 out;
	for (TomlTable::const_iterator it = table.begin(); it != table.end(); it++) {
		if (it->first == "cgi")
			out.cgi = _handle_map(it->second, Cgi::fromTomlValue);
		else if (it->first == "server")
			out.server = _handle_map(it->second, Server::fromTomlValue);
		else
			throw std::runtime_error(std::string("unknown key \"") + it->first +
									 "\" in top level table");
	}
	return out;
}

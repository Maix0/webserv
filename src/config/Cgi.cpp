/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/28 16:31:07 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <set>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/_ConfigHelper.hpp"
#include "toml/TomlValue.hpp"

Cgi Cgi::fromTomlValue(const TomlValue& toml) {
	const TomlTable&	  table = toml.getTable();
	Cgi					  out;
	std::set<std::string> seen;
	out.from_env = false;

	for (TomlTable::const_iterator it = table.begin(); it != table.end(); it++) {
		seen.insert(it->first);
		if (it->first == "binary")
			out.binary = it->second.getString();
		else if (it->first == "from_env")
			out.from_env = it->second.getBool();
		else
			throw std::runtime_error(std::string("unknown key \"") + it->first + "\" in Cgi table");
	}
	if (seen.count("binary") == 0)
		throw std::runtime_error("Missing key \"binary\" in cgi");
	return out;
}

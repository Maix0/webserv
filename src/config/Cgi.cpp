/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/14 10:43:52 by bgoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <set>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/_ConfigHelper.hpp"
#include "toml/Value.hpp"

using std::set;
using std::string;

namespace config {
	Cgi Cgi::fromTomlValue(const toml::Value& toml) {
		const toml::Table& table = toml.getTable();
		Cgi				   out;
		set<string>		   seen;
		out.from_env = false;

		for (toml::Table::const_iterator it = table.begin(); it != table.end(); it++) {
			try {
				seen.insert(it->first);
				if (it->first == "binary")
					out.binary = it->second.getString();
				else if (it->first == "from_env")
					out.from_env = it->second.getBool();
				else
					throw std::runtime_error("unknown key");
			} catch (const std::exception& e) {
				throw CgiParseError(string("\"") + it->first + "\" " + e.what());
			}
		}
		if (seen.count("binary") == 0)
			throw CgiParseError("missing key \"binary\" in cgi");
		return out;
	}
}  // namespace config

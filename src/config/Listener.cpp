/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 16:36:58 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/05 17:00:18 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <set>
#include <stdexcept>
#include "app/Logger.hpp"
#include "app/Socket.hpp"
#include "config/Config.hpp"
#include "toml/TomlValue.hpp"

Listener Listener::fromTomlValue(const TomlValue& toml) {
	const TomlTable&	  table = toml.getTable();
	Listener			  out;
	std::set<std::string> seen;

	for (TomlTable::const_iterator it = table.begin(); it != table.end(); it++) {
		try {
			seen.insert(it->first);
			if (it->first == "host")
				out.host = it->second.getString();
			else if (it->first == "port") {
				if (it->second.isInt()) {
					TomlNumber p = it->second.getInt();
					if (p < 0 || p > 65565)
						throw std::runtime_error("Port is not out of range (0-65565)");
					unsigned short up = static_cast<unsigned short>(p);
					out.port.push_back(up);
				} else {
					const TomlList& l = it->second.getList();
					if (l.empty())
						throw std::runtime_error("port can't be an empty list");
					for (TomlList::const_iterator it = l.begin(); it != l.end(); it++) {
						TomlNumber p = it->getInt();
						if (p < 0 || p > 65565)
							throw std::runtime_error("port is not out of range (0-65565)");
						unsigned short up = static_cast<unsigned short>(p);
						if (std::find(out.port.begin(), out.port.end(), Port(up)) != out.port.end())
							LOG(warn, "duplicate port in server (skipping...): " << up);
						else
							out.port.push_back(up);
					}
				}
			} else
				throw std::runtime_error("unknown key");
		} catch (const std::exception& e) {
			throw CgiParseError(std::string("\"") + it->first + "\" " + e.what());
		}
	}
	if (seen.count("host") == 0)
		throw CgiParseError("missing key \"host\" in cgi");
	if (seen.count("port") == 0)
		throw CgiParseError("missing key \"port\" in cgi");
	return out;
}

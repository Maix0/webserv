/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:40:19 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <exception>
#include <iostream>
#include <set>
#include <stdexcept>

#include "app/Logger.hpp"
#include "config/Config.hpp"
#include "config/_ConfigHelper.hpp"
#include "toml/Value.hpp"

namespace config {
Route Route::fromTomlValue(const toml::Value& toml) {
	const toml::Table& table = toml.getTable();
	Route			   out;

	out.listing	 = true;
	out.max_size = 16000000;

	for (toml::Table::const_iterator it = table.begin(); it != table.end(); it++) {
		try {
			if (it->first == "listing")
				out.listing = it->second.getBool();
			else if (it->first == "max_size") {
				if (it->second.getInt() < 0)
					throw std::runtime_error("can't be negative");
				out.max_size = it->second.getInt();
			} else if (it->first == "allowed") {
				if (it->second.isNull())
					out.allowed = Option<std::vector<std::string> >::None();
				else {
					const toml::List& l = it->second.getList();
					out.allowed			= Option<std::vector<std::string> >::Some();
					for (toml::List::const_iterator lit = l.begin(); lit != l.end(); lit++) {
						const std::string&				method = lit->getString();
						const std::vector<std::string>& lists  = out.allowed.get();

						if (std::find(lists.begin(), lists.end(), method) != lists.end())
							LOG(warn, "duplicate Method for route (skipping...): " << method);
						else
							out.allowed.get().push_back(method);
					}
				}
			} else if (it->first == "root") {
				if (it->second.isNull())
					out.root = Option<std::string>::None();
				else
					out.root = Option<std::string>::Some(it->second.getString());
			} else if (it->first == "post_directory") {
				if (it->second.isNull())
					out.post_dir = Option<std::string>::None();
				else
					out.post_dir = Option<std::string>::Some(it->second.getString());
			} else if (it->first == "index") {
				if (it->second.isNull())
					out.index = Option<std::string>::None();
				else
					out.index = Option<std::string>::Some(it->second.getString());
			} else if (it->first == "cgi")
				out.cgi = _handle_map(it->second, _toml_get_string);
			else if (it->first == "redirect") {
				if (it->second.isNull())
					out.redirect = Option<std::string>::None();
				else
					out.redirect = Option<std::string>::Some(it->second.getString());
			} else
				throw std::runtime_error("unknown key");
		} catch (const std::exception& e) {
			throw RouteParseError(std::string("\"") + it->first + "\" " + e.what());
		}
	}
	return out;
}
}  // namespace config

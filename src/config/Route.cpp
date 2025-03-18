/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/14 10:44:05 by bgoulard         ###   ########.fr       */
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

using std::string;
using std::vector;

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
						out.allowed = Option<vector<string> >::None();
					else {
						const toml::List& l = it->second.getList();
						out.allowed			= Option<vector<string> >::Some();
						for (toml::List::const_iterator lit = l.begin(); lit != l.end(); lit++) {
							const string&		  method = lit->getString();
							const vector<string>& lists	 = out.allowed.get();

							if (std::find(lists.begin(), lists.end(), method) != lists.end())
								LOG(warn, "duplicate Method for route (skipping...): " << method);
							else
								out.allowed.get().push_back(method);
						}
					}
				} else if (it->first == "root") {
					if (it->second.isNull())
						out.root = Option<string>::None();
					else
						out.root = Option<string>::Some(it->second.getString());
				} else if (it->first == "post_directory") {
					if (it->second.isNull())
						out.post_dir = Option<string>::None();
					else
						out.post_dir = Option<string>::Some(it->second.getString());
				} else if (it->first == "index") {
					if (it->second.isNull())
						out.index = Option<string>::None();
					else
						out.index = Option<string>::Some(it->second.getString());
				} else if (it->first == "cgi")
					out.cgi = _handle_map(it->second, _toml_get_string);
				else if (it->first == "redirect") {
					if (it->second.isNull())
						out.redirect = Option<string>::None();
					else
						out.redirect = Option<string>::Some(it->second.getString());
				} else
					throw std::runtime_error("unknown key");
			} catch (const std::exception& e) {
				throw RouteParseError(string("\"") + it->first + "\" " + e.what());
			}
		}
		return out;
	}
}  // namespace config

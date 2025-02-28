/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 15:40:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/28 16:59:33 by maiboyer         ###   ########.fr       */
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

Route Route::fromTomlValue(const TomlValue& toml) {
	const TomlTable& table = toml.getTable();
	Route			 out;

	out.listing	 = true;
	out.max_size = 16000000;

	for (TomlTable::const_iterator it = table.begin(); it != table.end(); it++) {
		if (it->first == "listing")
			out.listing = it->second.getBool();
		else if (it->first == "max_size") {
			if (it->second.getInt() < 0)
				throw std::runtime_error("max_size can't be negative");
			out.max_size = it->second.getInt();
		} else if (it->first == "allowed") {
			if (it->second.isNull())
				out.allowed = Option<std::vector<std::string> >::None();
			else {
				const TomlList& l = it->second.getList();
				out.allowed		  = Option<std::vector<std::string> >::Some();
				for (TomlList::const_iterator lit = l.begin(); lit != l.end(); lit++) {
					const std::string&				method = lit->getString();
					const std::vector<std::string>& lists  = out.allowed.get();

					if (std::find(lists.begin(), lists.end(), method) != lists.end())
						std::cerr << "Duplicate Method for route (skipping...): " << method
								  << std::endl;
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
				out.post_directory = Option<std::string>::None();
			else
				out.post_directory = Option<std::string>::Some(it->second.getString());
		} else if (it->first == "index") {
			if (it->second.isNull())
				out.index = Option<std::string>::None();
			else
				out.index = Option<std::string>::Some(it->second.getString());
		} else if (it->first == "cgi")
			out.cgi = _handle_map(it->second, _toml_get_string);
		else
			throw std::runtime_error(std::string("unknown key \"") + it->first +
									 "\" in Route table");
	}
	return out;
}

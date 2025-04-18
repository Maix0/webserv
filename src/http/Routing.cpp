/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Routing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:30:56 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/18 17:19:29 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "app/http/Routing.hpp"
#include "lib/IndexMap.hpp"
#include "runtime/Logger.hpp"

using std::string;
using std::vector;

vector<string> url_to_parts(const std::string& url) {
	vector<string>	  parts;
	std::stringstream ss(url);
	string			  part;
	while (std::getline(ss, part, '/')) {
		/// This allows us to find query strings
		/// if we find one, then the rest of the url isn't a route, but some kind of
		/// arguments, and we don't care.
		/// thus we just truncate that part, push to the vector containing the parts, and just
		/// leave.
		///
		/// otherwise, just push the thing :)
		if (part.find('?') != string::npos) {
			string::size_type idx = part.find('?');
			part.erase(idx);
			if (!part.empty())
				parts.push_back(part);
			break;
		}
		if (!part.empty())
			parts.push_back(part);
	}
	return parts;
}

void print_parts(std::string msg, const vector<string>& p) {
	vector<string>::const_iterator it = p.begin();
	std::stringstream			   out;

	out << "[";
	if (it != p.end())
		out << *(it++);
	for (; it != p.end(); it++)
		out << ", " << *it;
	out << "]";
	LOG(info, msg << ": " << out.str());
}

const config::Route* getRouteFor(const config::Server& server, const std::string& url) {
	vector<string> parts			   = url_to_parts(url);

	const config::Route* closest_match = NULL;
	int					 match_count   = 0;
	if (server.routes.count("/") != 0) {
		closest_match = &server.routes.at("/");
	}
	print_parts("url", parts);

	for (IndexMap<std::string, config::Route>::const_iterator it = server.routes.begin();
		 it != server.routes.end(); it++) {
		const vector<string>& route_parts = it->second.parts;
		print_parts("route", route_parts);
		// if the route is longer than the url, then it CAN'T be a match, then just skip
		if (parts.size() < route_parts.size())
			continue;
		// the route perfectly match the url, then yeah we found it for sure
		if (parts == route_parts) {
			closest_match = &it->second;
			// match_count	  = parts.size();
			break;
		}
		for (std::size_t i = 0; i < parts.size(); i++) {
			// we have exausted all segment of the route url, then we just check that we match
			// more segments than the current matching route
			if (i == route_parts.size()) {
				if ((int)i >= match_count) {
					match_count	  = i;
					closest_match = &it->second;
				}
				print_parts("update", route_parts);
				break;
			}
			// we just check that every segments matches
			if (parts[i] != route_parts[i])
				break;
		}
	}
	return closest_match;
}

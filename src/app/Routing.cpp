/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Routing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:30:56 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/14 17:35:54 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "app/Logger.hpp"
#include "app/Routing.hpp"

using std::string;
using std::vector;

namespace app {

	static vector<string> to_parts(const std::string& url) {
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

	const config::Route* getRouteFor(const config::Server& server, const std::string& url) {
		LOG(trace, "fetching route \" " << url << "\"for server " << server.name);
		vector<string> parts = to_parts(url);

		const config::Route* closest_match = NULL;
		int					 match_count   = 0;
		if (server.routes.count("/") != 0) {
			closest_match = &server.routes.at("/");
		}

		for (std::map<std::string, config::Route>::const_iterator it = server.routes.begin();
			 it != server.routes.end(); it++) {
			vector<string> route_parts = to_parts(it->first);
			// if the route is longer than the url, then it CAN'T be a match, then just skip
			if (parts.size() < route_parts.size())
				continue;
			// the route perfectly match the url, then yeah we found it for sure
			if (parts == route_parts) {
				closest_match = &it->second;
				match_count	  = parts.size();
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
					break;
				}
				// we just check that every segments matches
				if (parts[i] != route_parts[i])
					break;
			}
		}
		if (closest_match != NULL) {
			std::cout << "Found route " << closest_match->name << " for url " << url << std::endl;
		} else {
			std::cout << "Found no route for url " << url << std::endl;
		}

		return closest_match;
	}

};	// namespace app

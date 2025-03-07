/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _checkConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 21:50:04 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/07 23:28:48 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include "app/Context.hpp"
#include "app/Logger.hpp"
#include "config/Config.hpp"
#include "toml/Value.hpp"

namespace config {
	/// this function will change the `host` of every server
	static void _checkInvalidIpPorts(Config& config) {
		app::PortMap port_map;

		for (std::map<std::string, config::Server>::iterator sit = config.server.begin();
			 sit != config.server.end(); sit++) {
			struct addrinfo hints, *res, *p;

			memset(&hints, 0, sizeof(hints));
			hints.ai_family		 = AF_INET;		 // Allow IPv4
			hints.ai_socktype	 = SOCK_STREAM;	 // TCP

			const char* hostname = sit->second.bind_str.c_str();

			int			status	 = getaddrinfo(hostname, NULL, &hints, &res);
			if (status != 0) {
				LOG(err, "error resolving " << hostname << ": " << gai_strerror(status));
				throw std::runtime_error("failed to resolve hostname");
				return;
			}
			bool bound = false;
			for (p = res; p != NULL; p = p->ai_next) {
				if (res->ai_family == AF_INET) {
					struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
					app::Ip				ip	 = ipv4->sin_addr.s_addr;
					LOG(trace, "addrinfo: '" << hostname << "' = " << ip);
					sit->second.bind = ip;
					bound			 = true;
				}
			}
			if (!bound) {
				LOG(err, "failed to resolve any ip for host " << hostname);
				freeaddrinfo(res);
				throw std::runtime_error("failed to resolve hostname");
			}
			freeaddrinfo(res);

			// now we have the ips, put them in a map port->[ips] so we can check that there isn't
			// any overlap

			// if port == 0, then we don't care because OS will give us a port !
			if (sit->second.port == 0)
				continue;
			/// insert a set if none exists for the port
			if (port_map.count(sit->second.port) == 0)
				port_map[sit->second.port] = std::set<app::Ip>();

			port_map[sit->second.port].insert(sit->second.bind);
		}

		for (std::map<app::Port, std::set<app::Ip> >::iterator it = port_map.begin();
			 it != port_map.end(); it++) {
			if (it->second.size() > 1) {
				LOG(err, "Too many different ips tries to bind onto the port " << it->first);
				throw std::runtime_error("duplicate port for different ips");
			}
		}
		app::Context& ctx = app::Context::getInstance();
		ctx.getPortMap()  = port_map;
	}

	static void _checkUnknownCgi(const Config& config) {
		for (std::map<std::string, config::Server>::const_iterator sit = config.server.begin();
			 sit != config.server.end(); sit++) {
			for (std::map<std::string, config::Route>::const_iterator rit =
					 sit->second.routes.begin();
				 rit != sit->second.routes.end(); rit++) {
				for (std::map<std::string, std::string>::const_iterator cit =
						 rit->second.cgi.begin();
					 cit != rit->second.cgi.end(); cit++) {
					if (config.cgi.count(cit->second) == 0) {
						LOG(err, "cgi '" << cit->second << "' isn't known (server '" << sit->first
										 << "' route '" << rit->first << "')");
						throw std::runtime_error("Unknown CGI");
					}
				}
			}
		}
	}

	void checkConfig(Config& config) {
		_checkUnknownCgi(config);
		_checkInvalidIpPorts(config);
	}
}  // namespace config

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _checkConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 21:50:04 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/25 14:40:52 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include "app/State.hpp"
#include "app/http/Routing.hpp"
#include "config/Config.hpp"
#include "config/_ConfigHelper.hpp"
#include "lib/IndexMap.hpp"
#include "runtime/Logger.hpp"
#include "toml/Value.hpp"

using std::map;
using std::set;
using std::string;
using std::vector;

namespace config {
	static vector<string> _get_binary_paths(const string&					   binary,
											const Option<vector<string> /**/>& PATH) {
		vector<string> out;
		if (binary.find_first_of('/') != string::npos) {
			out.push_back(binary);
			return out;
		}
		if (!PATH.hasValue()) {
			LOG(fatal, "PATH isn't set but required");
			throw std::runtime_error("PATH not set");
		}
		const vector<string>& p = PATH.get();
		for (vector<string>::const_iterator it = p.begin(); it != p.end(); ++it) {
			out.push_back(*it + "/" + binary);
		}
		return out;
	}

	static void _checkInvalidCgi(Config& config, char** envp) {
		Option<vector<string> /**/> path;
		{
			char** envp_path = envp;
			char*  path_cstr = NULL;
			while (*envp_path) {
				string			  env = *envp_path;
				string::size_type eq  = env.find_first_of('=');
				string			  key = env.substr(0, eq);
				if (key == "PATH") {
					path_cstr = *envp_path;
					break;
				}
				envp_path++;
			}
			if (path_cstr != NULL) {
				string			  raw = path_cstr;
				string::size_type eq  = raw.find_first_of('=');
				std::stringstream ss(raw.substr(eq + 1));
				vector<string>	  out;
				while (std::getline(ss, raw, ':'))
					out.push_back(raw);
				path = out;
			}
		}
		// LOG(trace, "using path=" << path << "");

		bool error = false;
		for (map<string, Cgi>::iterator cit = config.cgi.begin(); cit != config.cgi.end(); ++cit) {
			const string& name = cit->first;
			Cgi&		  cgi  = cit->second;

			(void)(name);  // used by log macro

			if (cgi.from_env) {
				char** envp2 = envp;
				bool   found = false;
				while (*envp2) {
					string			  env = *envp2;
					string::size_type eq  = env.find_first_of('=');
					string			  key = env.substr(0, eq);
					if (key == cgi.binary) {
						cgi.binary	 = env.substr(eq + 1);
						cgi.from_env = false;
						found		 = true;
						break;
					}
					envp2++;
				}
				if (!found)
					LOG(err, "cgi '" << name << "' was declared as using a variable ('"
									 << cgi.binary << "') to get path, but no such var is set");
				error |= !found;
			}
			if (!cgi.from_env) {
				vector<string> possible_paths = _get_binary_paths(cgi.binary, path);
				Option<string> found;
				for (vector<string>::const_iterator it = possible_paths.begin();
					 it != possible_paths.end(); ++it) {
					if (!access(it->c_str(), X_OK)) {
						found = *it;
					}
				}
				if (!found.hasValue()) {
					LOG(err, "cgi '" << name << "': found no executable");
					error |= true;
				} else {
					cgi.binary = found.get();
					LOG(trace, "cgi '" << name << "': using " << cgi.binary);
				}
			}
		}
		if (error) {
			throw std::runtime_error("failed to get CGI paths");
		}
	};

	/// this function will change the `host` of every server
	static void _checkInvalidIpPorts(Config& config) {
		PortMap port_map;

		for (IndexMap<string, config::Server>::iterator sit = config.server.begin();
			 sit != config.server.end(); sit++) {
			struct addrinfo hints, *res, *p;

			memset(&hints, 0, sizeof(hints));
			hints.ai_family		 = AF_INET;		 // Allow IPv4
			hints.ai_socktype	 = SOCK_STREAM;	 // TCP

			const char* hostname = sit->second.bind_str.c_str();

			int status			 = getaddrinfo(hostname, NULL, &hints, &res);
			if (status != 0) {
				LOG(err, "error resolving " << hostname << ": " << gai_strerror(status));
				throw std::runtime_error("failed to resolve hostname");
				return;
			}
			bool bound = false;
			for (p = res; p != NULL; p = p->ai_next) {
				if (res->ai_family == AF_INET) {
					struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
					Ip					ip	 = ipv4->sin_addr.s_addr;
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

			// now we have the ips, put them in a map port->[ips] so we can check that there
			// isn't any overlap

			/// insert a set if none exists for the port
			if (port_map.count(sit->second.port) == 0)
				port_map[sit->second.port] = set<Ip>();

			port_map[sit->second.port].insert(sit->second.bind);
		}

		for (map<Port, set<Ip> /**/>::iterator it = port_map.begin(); it != port_map.end(); it++) {
			if (it->second.size() > 1) {
				LOG(err, "Too many different ips tries to bind onto the port " << it->first);
				throw std::runtime_error("duplicate port for different ips");
			}
		}
		State& ctx		 = State::getInstance();
		ctx.getPortMap() = port_map;
	}

	static void _checkUnknownCgi(Config& config) {
		for (ServerIterator sit = config.server.begin(); sit != config.server.end(); sit++) {
			for (RouteIterator rit = sit->second.routes.begin(); rit != sit->second.routes.end();
				 rit++) {
				for (map<string, string>::iterator cit = rit->second.cgi.begin();
					 cit != rit->second.cgi.end(); cit++) {
					if (cit->first.empty()) {
						LOG(err, "cgi '" << cit->second
										 << "' is matching an empty extension (server '"
										 << sit->first << "' route '" << rit->first << "')");
						throw std::runtime_error("Invalid CGI");
					}
					if (config.cgi.count(cit->second) == 0) {
						LOG(err, "cgi '" << cit->second << "' isn't known (server '" << sit->first
										 << "' route '" << rit->first << "')");
						throw std::runtime_error("Unknown CGI");
					}
				}
			}
		}

		for (ServerIterator sit = config.server.begin(); sit != config.server.end(); sit++) {
			for (RouteIterator rit = sit->second.routes.begin(); rit != sit->second.routes.end();
				 rit++) {
				std::vector<std::pair<std::string, std::string> > v(rit->second.cgi.begin(),
																	rit->second.cgi.end());
				for (std::vector<std::pair<std::string, std::string> >::iterator it = v.begin();
					 it != v.end(); it++) {
					if (it->first[0] != '.')
						it->first.insert(it->first.begin(), 1, '.');
				}

				rit->second.cgi = std::map<std::string, std::string>(v.begin(), v.end());
			}
		}
	}

	static void _buildPortServerMap(Config& config) {
		PortServerMap out;
		for (IndexMap<std::string, Server>::iterator it = config.server.begin();
			 it != config.server.end(); it++) {
			if (out.count(it->second.port) == 0)
				out[it->second.port] = std::vector<Server*>();
			out[it->second.port].push_back(&it->second);
		}
		bool error = false;
		for (PortServerMap::iterator pit = out.begin(); pit != out.end(); pit++) {
			std::set<std::string> hostnames;
			for (vector<Server*>::iterator vit = pit->second.begin(); vit != pit->second.end();
				 vit++) {
				Server& serv = **vit;
				if (!serv.hostname.hasValue())
					continue;
				if (hostnames.count(serv.hostname.get()) != 0) {
					LOG(err, "server " << serv.name
									   << " has an hostname that is already present for port "
									   << pit->first << " (" << serv.hostname.get() << ")");
					error = true;
					continue;
				}
				hostnames.insert(serv.hostname.get());
			}
		}
		if (error)
			throw std::runtime_error("Duplicate hostname for same port");
		State::getInstance().getPortServerMap() = out;
	};

	static void _setupRoutes(Config& config) {
		bool error = false;
		for (IndexMap<string, Server>::iterator sit = config.server.begin();
			 sit != config.server.end(); sit++) {
			std::set<std::vector<string> > route_seens;
			for (IndexMap<string, Route>::iterator route = sit->second.routes.begin();
				 route != sit->second.routes.end(); route++) {
				vector<string> parts = url_to_parts(route->first);
				if (route_seens.count(parts) != 0) {
					LOG(err, "Duplicate route for server "
								 << sit->first << " (offending route: " << route->first << ")");
					error = true;
				}
				route->second.parts = parts;
				route_seens.insert(parts);
			}
		}
		if (error)
			throw std::runtime_error("Error in routes");
	}

	void checkConfig(Config& config, char** envp) {
		_buildPortServerMap(config);
		_checkUnknownCgi(config);
		_checkInvalidCgi(config, envp);
		_checkInvalidIpPorts(config);
		_setupRoutes(config);
	}
}  // namespace config

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   _checkConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 21:50:04 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 15:42:51 by maiboyer         ###   ########.fr       */
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
#include "app/Context.hpp"
#include "app/Logger.hpp"
#include "config/Config.hpp"
#include "config/_ConfigHelper.hpp"
#include "toml/Value.hpp"

namespace config {
	static std::vector<std::string> _get_binary_paths(
		const std::string&							 binary,
		const Option<std::vector<std::string> /**/>& PATH) {
		std::vector<std::string> out;
		if (binary.find_first_of('/') != std::string::npos) {
			out.push_back(binary);
			return out;
		}
		if (!PATH.hasValue()) {
			LOG(fatal, "PATH isn't set but required");
			throw std::runtime_error("PATH not set");
		}
		const std::vector<std::string>& p = PATH.get();
		for (std::vector<std::string>::const_iterator it = p.begin(); it != p.end(); ++it) {
			out.push_back(*it + "/" + binary);
		}
		return out;
	}

	static void _checkInvalidCgi(Config& config, char** envp) {
		Option<std::vector<std::string> /**/> path;
		{
			char** envp_path = envp;
			char*  path_cstr = NULL;
			while (*envp_path) {
				std::string			   env = *envp_path;
				std::string::size_type eq  = env.find_first_of('=');
				std::string			   key = env.substr(0, eq);
				if (key == "PATH") {
					path_cstr = *envp_path;
					break;
				}
				envp_path++;
			}
			if (path_cstr != NULL) {
				std::string				 raw = path_cstr;
				std::string::size_type	 eq	 = raw.find_first_of('=');
				std::stringstream		 ss(raw.substr(eq + 1));
				std::vector<std::string> out;
				while (std::getline(ss, raw, ':'))
					out.push_back(raw);
				path = out;
			}
		}
		// LOG(trace, "using path=" << path << "");

		bool error = false;
		for (std::map<std::string, Cgi>::iterator cit = config.cgi.begin(); cit != config.cgi.end();
			 ++cit) {
			const std::string& name = cit->first;
			Cgi&			   cgi	= cit->second;

			(void)(name);  // used by log macro

			if (cgi.from_env) {
				char** envp2 = envp;
				bool   found = false;
				while (*envp2) {
					std::string			   env = *envp2;
					std::string::size_type eq  = env.find_first_of('=');
					std::string			   key = env.substr(0, eq);
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
				std::vector<std::string> possible_paths = _get_binary_paths(cgi.binary, path);
				Option<std::string>		 found;
				for (std::vector<std::string>::const_iterator it = possible_paths.begin();
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

			// now we have the ips, put them in a map port->[ips] so we can check that there
			// isn't any overlap

			/// insert a set if none exists for the port
			if (port_map.count(sit->second.port) == 0)
				port_map[sit->second.port] = std::set<app::Ip>();

			port_map[sit->second.port].insert(sit->second.bind);
		}

		for (std::map<app::Port, std::set<app::Ip> /**/>::iterator it = port_map.begin();
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

	void checkConfig(Config& config, char** envp) {
		_checkUnknownCgi(config);
		_checkInvalidCgi(config, envp);
		_checkInvalidIpPorts(config);
	}
}  // namespace config

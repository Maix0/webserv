/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:14:09 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/08 18:25:02 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Context.hpp"
#include <vector>
#include "app/Logger.hpp"

namespace app {

	Context Context::INSTANCE;

	Context::Context() {
		LOG(trace, "creating context singleton");
	}

	Context::~Context() {
		LOG(trace, "destroying context singleton");
	}

	Context& Context::getInstance() {
		return (Context::INSTANCE);
	}

	void Context::openAllSockets() {
		for (PortMap::const_iterator pit = this->port_map.begin(); pit != this->port_map.end();
			 pit++) {
			const Port&			port = pit->first;
			const std::set<Ip>& ips	 = pit->second;

			for (std::set<Ip>::const_iterator iit = ips.begin(); iit != ips.end(); iit++) {
				const Ip& ip = *iit;

				if (this->sockets.count(ip) == 0)
					this->sockets[ip] = std::vector<Shared<Socket> >();
				LOG(trace, "creating socket for " << ip << ":" << port);
				this->sockets[ip].push_back(new Socket(ip, port));
			}
		}
		LOG(info, "all sockets are opened: " << this->sockets.size() << " sockets");
	};
};	// namespace app

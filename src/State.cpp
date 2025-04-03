/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:14:09 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 13:40:26 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>

#include "app/State.hpp"
#include "runtime/Logger.hpp"

using std::set;
using std::vector;

State State::INSTANCE;

State::State() {
	LOG(trace, "creating context singleton");
}

State::~State() {
	LOG(trace, "destroying context singleton");
}

State& State::getInstance() {
	return (State::INSTANCE);
}

void State::openAllSockets() {
	for (PortMap::const_iterator pit = this->port_map.begin(); pit != this->port_map.end(); pit++) {
		const Port&	   port = pit->first;
		const set<Ip>& ips	= pit->second;

		for (set<Ip>::const_iterator iit = ips.begin(); iit != ips.end(); iit++) {
			const Ip& ip = *iit;

			if (this->sockets.count(ip) == 0)
				this->sockets[ip] = vector<Rc<Socket> >();
			LOG(trace, "creating socket for " << ip << ":" << port);
			this->sockets[ip].push_back(new Socket(ip, port));
		}
	}
	LOG(info, "all sockets are opened: " << this->sockets.size() << " sockets");
};

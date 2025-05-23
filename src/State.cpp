/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:14:09 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/29 10:41:49 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>

#include "app/State.hpp"
#include "runtime/Logger.hpp"

void dbg() {

}

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
			Rc<Socket> sock(Functor3<Socket, Ip, Port, config::Server*>(
								ip, port, this->port_server_map.at(port)[0]),
							RCFUNCTOR);
			this->sockets[ip].push_back(sock);
		}
	}
	LOG(info, "all sockets are opened: " << this->sockets.size() << "opened sockets");
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:07:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/07 23:27:59 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>
#include "app/Socket.hpp"
#include "config/Config.hpp"

namespace app {
	typedef std::map<std::string, std::vector<Socket> > SocketList;
	typedef std::map<app::Port, std::set<app::Ip> >		PortMap;

	class Context {
	private:
		Context();
		~Context();
		// No Copy operator/assignement operator since this is a Singleton...

		static Context								INSTANCE;

		std::map<app::Port, std::set<app::Ip> >		port_map;

		std::map<std::string, std::vector<Socket> > sockets;
		config::Config								config;

	public:
		static Context& getInstance();

		config::Config& getConfig() { return this->config; };
		SocketList&		getSockets() { return this->sockets; };
		PortMap&		getPortMap() { return this->port_map; };
	};
}  // namespace app

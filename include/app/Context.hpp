/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:07:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 14:00:24 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <set>
#include <vector>
#include "app/Epoll.hpp"
#include "app/Shared.hpp"
#include "app/Socket.hpp"
#include "config/Config.hpp"

namespace app {
	typedef std::map<app::Ip, std::vector<app::Shared<app::Socket> > > SocketList;
	typedef std::map<app::Port, std::set<app::Ip> >					   PortMap;

	class Context {
	private:
		Context();
		~Context();
		// No Copy operator/assignement operator since this is a Singleton...

		static Context										  INSTANCE;

		std::map<app::Port, std::set<app::Ip> >				  port_map;

		std::map<app::Ip, std::vector<app::Shared<Socket> > > sockets;
		config::Config										  config;
		app::Epoll											  epoll;

	public:
		static Context& getInstance();

		config::Config& getConfig() { return this->config; };
		SocketList&		getSockets() { return this->sockets; };
		PortMap&		getPortMap() { return this->port_map; };

		void			openAllSockets();
	};
}  // namespace app

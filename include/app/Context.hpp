/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:07:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/13 16:38:48 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <set>
#include <vector>
#include "app/Connection.hpp"
#include "app/Shared.hpp"
#include "app/Socket.hpp"
#include "config/Config.hpp"

namespace app {
	extern Shared<bool> do_shutdown;

	typedef std::map<app::Ip, std::vector<app::Shared<app::Socket> > > SocketList;
	typedef std::map<app::Port, std::set<app::Ip> >					   PortMap;
	typedef std::vector<app::Shared<Connection> >					   ConnectionList;

	class Context {
		private:
			Context();
			~Context();
			// No Copy operator/assignement operator since this is a Singleton...

			static Context INSTANCE;

			ConnectionList conns;
			PortMap		   port_map;
			SocketList	   sockets;

			config::Config			config;
			Option<Shared<Socket> > shutdown;

		public:
			static Context& getInstance();

			config::Config&			 getConfig() { return this->config; };
			SocketList&				 getSockets() { return this->sockets; };
			PortMap&				 getPortMap() { return this->port_map; };
			ConnectionList&			 getConnections() { return this->conns; };
			Option<Shared<Socket> >& getShutdown() { return this->shutdown; };

			void openAllSockets();
	};
}  // namespace app

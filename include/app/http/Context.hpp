/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:07:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/02 15:02:43 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <set>
#include <vector>
#include "app/net/Connection.hpp"
#include "lib/Rc.hpp"
#include "app/net/Socket.hpp"
#include "config/Config.hpp"

extern Rc<bool> do_shutdown;

typedef std::map<Ip, std::vector<Rc<Socket> > > SocketList;
typedef std::map<Port, std::set<Ip> >					   PortMap;
typedef std::vector<Rc<Connection> >					   ConnectionList;
typedef std::map<Port, std::vector<config::Server*> >		   PortServerMap;

class Context {
	private:
		Context();
		~Context();
		// No Copy operator/assignement operator since this is a Singleton...

		static Context INSTANCE;

		ConnectionList			conns;
		Option<Rc<Socket> > shutdown;
		PortMap					port_map;
		PortServerMap			port_server_map;
		SocketList				sockets;
		config::Config			config;

	public:
		static Context& getInstance();

		config::Config&			 getConfig() { return this->config; };
		SocketList&				 getSockets() { return this->sockets; };
		PortMap&				 getPortMap() { return this->port_map; };
		ConnectionList&			 getConnections() { return this->conns; };
		Option<Rc<Socket> >& getShutdown() { return this->shutdown; };
		PortServerMap			 getPortServerMap() { return this->port_server_map; };

		void openAllSockets();
};

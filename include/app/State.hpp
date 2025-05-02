/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:07:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/02 11:10:04 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <set>
#include <vector>
#include "app/http/CgiOutput.hpp"
#include "app/net/Connection.hpp"
#include "app/net/Socket.hpp"
#include "config/Config.hpp"
#include "lib/Rc.hpp"

extern bool do_shutdown;

typedef std::map<Ip, std::vector<Rc<Socket> > >		  SocketList;
typedef std::map<Port, std::set<Ip> >				  PortMap;
typedef std::vector<Rc<Connection> >				  ConnectionList;
typedef std::map<Port, std::vector<config::Server*> > PortServerMap;
typedef std::vector<Rc<CgiOutput> >					  CgiList;

struct ChildStatus {
		bool is_finished;
		int	 exit_code;

		ChildStatus() : is_finished(false), exit_code(-1) {}
		ChildStatus(const ChildStatus& rhs) { *this = rhs; };
		~ChildStatus() {}
		ChildStatus& operator=(const ChildStatus& rhs) {
			if (this != &rhs) {
				this->is_finished = rhs.is_finished;
				this->exit_code	  = rhs.exit_code;
			}
			return (*this);
		}
};

class State {
	private:
		State();
		~State();

		static State INSTANCE;

		ConnectionList				 conns;
		Option<Rc<Socket> >			 shutdown;
		PortMap						 port_map;
		PortServerMap				 port_server_map;
		SocketList					 sockets;
		config::Config				 config;
		CgiList						 cgis;
		char**						 envp;
		IndexMap<pid_t, ChildStatus> childs;

	public:
		static State& getInstance();

		CgiList&					  getCgis() { return this->cgis; };
		config::Config&				  getConfig() { return this->config; };
		SocketList&					  getSockets() { return this->sockets; };
		PortMap&					  getPortMap() { return this->port_map; };
		ConnectionList&				  getConnections() { return this->conns; };
		Option<Rc<Socket> >&		  getShutdown() { return this->shutdown; };
		PortServerMap&				  getPortServerMap() { return this->port_server_map; };
		char**						  getEnv() { return this->envp; };
		IndexMap<pid_t, ChildStatus>& getChildStatus() { return this->childs; };

		void setEnv(char** envp) { this->envp = envp; };

		void openAllSockets();
};

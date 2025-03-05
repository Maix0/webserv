/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:07:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/05 21:13:03 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>
#include <vector>
#include "app/Socket.hpp"
#include "config/Config.hpp"

typedef std::map<std::string, std::vector<Socket> > SocketList;

class Context {
private:
	Context();
	~Context();
	// No Copy operator/assignement operator since this is a Singleton...

	static Context								INSTANCE;

	std::map<std::string, std::vector<Socket> > sockets;
	Config										config;

public:
	static Context& getInstance();

	Config&			getConfig() { return this->config; };
	SocketList&		getSockets() { return this->sockets; };
};

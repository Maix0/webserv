/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 13:36:52 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/04 13:57:01 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ostream>
#include <string>

struct Ip {
	unsigned int inner;
};

std::ostream& operator<<(std::ostream& o, const Ip& rhs);

class Socket {
private:
	int			   fd;
	unsigned short port;
	Ip			   host;

public:
	Socket();
	Socket(const Ip& host, unsigned short port);
	~Socket();

	int			   getFd();
	unsigned short getPort();
	Ip			   getHost();
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 13:36:52 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/05 22:03:44 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ostream>
#include <string>

struct Ip {
	unsigned int		 inner;

	friend std::ostream& operator<<(std::ostream& o, const Ip& rhs) {
		unsigned char* ptr = (unsigned char*)&rhs.inner;
		o << (unsigned int)ptr[0] << "." << (unsigned int)ptr[1] << "." << (unsigned int)ptr[2]
		  << "." << (unsigned int)ptr[3];

		return (o);
	}

	Ip() : inner(0) {};
	Ip(unsigned int raw) : inner(raw) {};
	~Ip() {};
	Ip(const Ip& rhs) : inner(rhs.inner) {};
	Ip& operator=(const Ip& rhs) {
		if (this != &rhs)
			this->inner = rhs.inner;
		return (*this);
	};
};

struct Port {
	unsigned short inner;
	Port(unsigned short i) : inner(i) {};
	~Port() {};
	Port(const Port& rhs) : inner(rhs.inner) {};
	Port& operator=(const Port& rhs) {
		if (this != &rhs)
			this->inner = rhs.inner;
		return (*this);
	};

	bool				 operator==(Port& rhs) { return (this->inner == rhs.inner); };
	bool				 operator==(const Port& rhs) const { return (this->inner == rhs.inner); };
	bool				 operator!=(Port& rhs) { return (this->inner != rhs.inner); };
	bool				 operator!=(const Port& rhs) const { return (this->inner != rhs.inner); };

	friend std::ostream& operator<<(std::ostream& o, const Port& rhs) { return (o << rhs.inner); }
};

class Socket {
private:
	int			fd;
	Port		port;
	Ip			host_ip;
	std::string host_str;

public:
	Socket();
	Socket(const std::string& host, Port port);
	~Socket();

	int				   getFd() { return (this->fd); };
	Port			   getPort() { return (this->port); };
	Ip				   getHost() { return (this->host_ip); };
	const std::string& getHostStr() const { return (this->host_str); };
};

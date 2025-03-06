/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 13:36:52 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:58:31 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ostream>
#include <string>

namespace app {
struct Ip {
	unsigned int inner;

	Ip() : inner(0) {};
	Ip(unsigned int raw) : inner(raw) {};
	~Ip() {};
	Ip(const Ip& rhs) : inner(rhs.inner) {};
	Ip& operator=(const Ip& rhs) {
		if (this != &rhs)
			this->inner = rhs.inner;
		return (*this);
	};

	bool				 operator==(const Ip& rhs) const { return (this->inner == rhs.inner); };
	bool				 operator>(const Ip& rhs) const { return (this->inner > rhs.inner); };

	bool				 operator<(const Ip& rhs) const { return (rhs > *this); };
	bool				 operator<=(const Ip& rhs) const { return !(*this > rhs); };
	bool				 operator>=(const Ip& rhs) const { return !(*this < rhs); };
	bool				 operator!=(const Ip& rhs) const { return !(*this == rhs); };

	friend std::ostream& operator<<(std::ostream& o, const Ip& rhs) {
		unsigned char* ptr = (unsigned char*)&rhs.inner;
		o << (unsigned int)ptr[0] << "." << (unsigned int)ptr[1] << "." << (unsigned int)ptr[2]
		  << "." << (unsigned int)ptr[3];

		return (o);
	}
};

struct Port {
	unsigned short inner;
	Port(unsigned short raw) : inner(raw) {};
	~Port() {};
	Port(const Port& rhs) : inner(rhs.inner) {};
	Port& operator=(const Port& rhs) {
		if (this != &rhs)
			this->inner = rhs.inner;
		return (*this);
	};

	bool				 operator==(const Port& rhs) const { return (this->inner == rhs.inner); };
	bool				 operator>(const Port& rhs) const { return (this->inner > rhs.inner); };

	bool				 operator<(const Port& rhs) const { return (rhs > *this); };
	bool				 operator<=(const Port& rhs) const { return !(*this > rhs); };
	bool				 operator>=(const Port& rhs) const { return !(*this < rhs); };
	bool				 operator!=(const Port& rhs) const { return !(*this == rhs); };

	friend std::ostream& operator<<(std::ostream& o, const Port& rhs) { return (o << rhs.inner); }
};

class Socket {
private:
	int			fd;
	Port		port;
	Ip			host_ip;
	std::string host_str;

public:
	static const int BACKLOG = 5;

	Socket();
	Socket(const std::string& host, Port port);
	~Socket();

	int				   getFd() { return (this->fd); };
	Port			   getPort() { return (this->port); };
	Ip				   getHost() { return (this->host_ip); };
	const std::string& getHostStr() const { return (this->host_str); };
};
}  // namespace app

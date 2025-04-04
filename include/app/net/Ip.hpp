/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ip.hpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 16:27:22 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/04 16:27:35 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

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

		bool operator==(const Ip& rhs) const { return (this->inner == rhs.inner); };
		bool operator>(const Ip& rhs) const { return (this->inner > rhs.inner); };

		bool operator<(const Ip& rhs) const { return (rhs > *this); };
		bool operator<=(const Ip& rhs) const { return !(*this > rhs); };
		bool operator>=(const Ip& rhs) const { return !(*this < rhs); };
		bool operator!=(const Ip& rhs) const { return !(*this == rhs); };

		friend std::ostream& operator<<(std::ostream& o, const Ip& rhs) {
			unsigned char* ptr = (unsigned char*)&rhs.inner;
			o << (unsigned int)ptr[0] << "." << (unsigned int)ptr[1] << "." << (unsigned int)ptr[2]
			  << "." << (unsigned int)ptr[3];

			return (o);
		}
};

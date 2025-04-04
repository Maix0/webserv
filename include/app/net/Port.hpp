/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Port.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 16:27:22 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/04 16:27:47 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

struct Port {
		unsigned short inner;
		Port() : inner(0) {};
		Port(unsigned short raw) : inner(raw) {};
		~Port() {};
		Port(const Port& rhs) : inner(rhs.inner) {};
		Port& operator=(const Port& rhs) {
			if (this != &rhs)
				this->inner = rhs.inner;
			return (*this);
		};

		bool operator==(const Port& rhs) const { return (this->inner == rhs.inner); };
		bool operator>(const Port& rhs) const { return (this->inner > rhs.inner); };

		bool operator<(const Port& rhs) const { return (rhs > *this); };
		bool operator<=(const Port& rhs) const { return !(*this > rhs); };
		bool operator>=(const Port& rhs) const { return !(*this < rhs); };
		bool operator!=(const Port& rhs) const { return !(*this == rhs); };

		friend std::ostream& operator<<(std::ostream& o, const Port& rhs) {
			return (o << rhs.inner);
		}
};

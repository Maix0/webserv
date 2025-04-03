/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Time.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 19:26:51 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 19:30:00 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

struct Time {
		unsigned long long inner;

		bool operator==(const Time& rhs) const { return this->inner == rhs.inner; }
		bool operator!=(const Time& rhs) const { return this->inner != rhs.inner; }
		bool operator>(const Time& rhs) const { return this->inner > rhs.inner; }
		bool operator<(const Time& rhs) const { return this->inner < rhs.inner; }
		bool operator>=(const Time& rhs) const { return this->inner >= rhs.inner; }
		bool operator<=(const Time& rhs) const { return this->inner <= rhs.inner; }

		bool operator==(Time& rhs) { return this->inner == rhs.inner; }
		bool operator!=(Time& rhs) { return this->inner != rhs.inner; }
		bool operator>(Time& rhs) { return this->inner > rhs.inner; }
		bool operator<(Time& rhs) { return this->inner < rhs.inner; }
		bool operator>=(Time& rhs) { return this->inner >= rhs.inner; }
		bool operator<=(Time& rhs) { return this->inner <= rhs.inner; }

		static Time now();
};

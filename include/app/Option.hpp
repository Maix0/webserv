/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Option.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 14:52:32 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/28 17:24:39 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <ostream>
#include <stdexcept>

template <typename T>
class Option {
private:
	T* value;

public:
	Option(const T& v) : value(new T(v)) {};
	Option(void) : value(NULL) {};

	Option(const Option& rhs) : value(NULL) {
		if (rhs.value != NULL)
			this->value = new T(*rhs.value);
	}

	Option& operator=(const Option& rhs) {
		if (this != &rhs) {
			this->~Option();
			if (rhs.value != NULL)
				this->value = new T(*rhs.value);
		}
		return (*this);
	}

	~Option() {
		if (this->value)
			delete this->value;
	}

	void insert(const T& val) {
		this->~Option();
		this->value = new T(val);
	}

	bool	 hasValue() const { return (this->value != NULL); }
	const T* raw() const { return this->value; }
	T*		 raw() { return this->value; }

	const T& get() const {
		if (this->value == NULL)
			throw std::runtime_error("tried to get None");
		return *this->value;
	}
	T& get() {
		if (this->value == NULL)
			throw std::runtime_error("tried to get None");
		return *this->value;
	}

	static Option None() { return Option(); }
	static Option Some(const T& val) { return Option(val); }
	static Option Some() { return Option(T()); }
};

template <typename T>
std::ostream& operator<<(std::ostream& o, const Option<T>& rhs) {
	if (rhs.hasValue())
		o << "Some(" << rhs.get() << ")";
	else
		o << "None";
	return (o);
}

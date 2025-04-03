/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Option.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 14:52:32 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 13:55:38 by maiboyer         ###   ########.fr       */
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
				this->remove();
				if (rhs.value != NULL)
					this->value = new T(*rhs.value);
			}
			return (*this);
		}

		~Option() { this->remove(); }

		void insert(const T& val) {
			if (this->value != NULL)
				delete this->value;
			this->value = new T(val);
		}

		void remove() {
			if (this->value != NULL)
				delete this->value;
			this->value = NULL;
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

		const T& get_or(const T& default_value) const {
			if (this->value == NULL)
				return default_value;
			return *this->value;
		}

		T& get_or(T& default_value) {
			if (this->value == NULL)
				return default_value;
			return *this->value;
		}

		T& get_or_insert() {
			if (this->value == NULL)
				this->value = new T();
			return (*this->value);
		}

		T& get_or_insert(const T& val) {
			if (this->value == NULL)
				this->value = new T(val);
			return (*this->value);
		}
};

template <typename T>
std::ostream& operator<<(std::ostream& o, const Option<T>& rhs) {
	if (rhs.hasValue())
		o << "Some(" << rhs.get() << ")";
	else
		o << "None";
	return (o);
}

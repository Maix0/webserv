/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shared.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:52:34 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/08 18:24:18 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <stdexcept>
namespace app {

	template <typename T>
	class Shared {
	private:
		struct SharedData {
			std::size_t ref;
			T*			value;
		};
		SharedData* ptr;

	public:
		Shared() {
			this->ptr	   = new SharedData();
			this->ptr->ref = 1;
			new (&this->ptr->value) T();
		}

		Shared(T* val) {
			if (val == NULL)
				throw std::runtime_error("Tried to init a shared ptr from NULL");
			this->ptr		 = new SharedData();
			this->ptr->ref	 = 1;
			this->ptr->value = val;
		}

		~Shared() {
			if (--this->ptr->ref == 0) {
				delete this->ptr->value;
				delete this->ptr;
			}
		}

		Shared(const Shared& rhs) {
			this->ptr = rhs.ptr;
			this->ptr->ref++;
		}
		Shared& operator=(const Shared& rhs) {
			if (this != &rhs) {
				this->ptr = rhs.ptr;
				this->ptr->ref++;
			}
			return (*this);
		}

		T&		 operator*() { return (*this->ptr->value); }
		const T& operator*() const { return (*this->ptr->value); }
		T*		 operator->() { return (this->ptr->value); }
		const T* operator->() const { return (this->ptr->value); }
	};

}  // namespace app

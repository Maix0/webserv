/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shared.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:52:34 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/15 10:11:19 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <strings.h>
#include <cassert>
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

			Shared(SharedData* ptr) : ptr(ptr) {
				if (this->ptr == NULL)
					throw std::runtime_error("shared.ptr == NULL");
			};

			inline void assert_valid() const { assert(this->ptr && this->ptr->ref > 0); };

		public:
			// Opaque type. Represent Shared::SharedData
			struct RawShared;
			Shared() {
				this->ptr	   = new SharedData();
				this->ptr->ref = 1;
				new (&this->ptr->value) T();
				this->assert_valid();
			}

			Shared(T* val) {
				if (val == NULL)
					throw std::runtime_error("Tried to init a shared ptr from NULL");
				this->ptr		 = new SharedData();
				this->ptr->ref	 = 1;
				this->ptr->value = val;
				this->assert_valid();
			}

			~Shared() {
				this->assert_valid();
				if (this->ptr == NULL)
					throw std::runtime_error("shared.ptr == NULL");
				if (--this->ptr->ref == 0) {
					delete this->ptr->value;
					delete this->ptr;
					this->ptr = NULL;
				}
			}

			Shared(const Shared& rhs) {
				rhs.assert_valid();
				this->ptr = rhs.ptr;
				if (this->ptr == NULL)
					throw std::runtime_error("shared.ptr == NULL");
				this->ptr->ref++;
			}
			Shared& operator=(const Shared& rhs) {
				rhs.assert_valid();
				this->assert_valid();
				if (this != &rhs) {
					{
						this->ptr->ref--;
						Shared cpy = *this;
						// hello yes run the destructor pls
						(void)(cpy);
					}
					this->ptr = rhs.ptr;
					this->ptr->ref++;
				}
				return (*this);
			}

			T&		 operator*() { return (this->assert_valid(), *this->ptr->value); }
			const T& operator*() const { return (this->assert_valid(), *this->ptr->value); }
			T*		 operator->() { return (this->assert_valid(), this->ptr->value); }
			const T* operator->() const { return (this->assert_valid(), this->ptr->value); }

			// GetRaw
			RawShared* getRaw() {
				this->assert_valid();
				return ((RawShared*)this->ptr);
			}

			// GetRaw
			static Shared fromRaw(RawShared* raw) {
				if (raw == NULL)
					throw std::runtime_error("Tried to init a shared ptr from raw NULL");
				SharedData* raw_cast = (SharedData*)(raw);

				return Shared(raw_cast);
			}

			template <typename U>
			Shared<U> cast() {
				this->assert_valid();
				U* ptr = (this->ptr->value);
				assert((void*)ptr == (void*)this->ptr->value);
				this->ptr->ref++;
				RawShared* raw = this->getRaw();
				return Shared<U>::fromRaw((typename Shared<U>::RawShared*)raw);
			}

			template <typename U>
			Shared<U> try_cast() {
				this->assert_valid();
				T& val		= *this->ptr->value;
				U& val_cast = dynamic_cast<U&>(val);
				assert(&val == &val_cast);
				this->ptr->ref++;
				RawShared* raw = this->getRaw();
				return Shared<U>::fromRaw((typename Shared<U>::RawShared*)raw);
			}
	};

}  // namespace app

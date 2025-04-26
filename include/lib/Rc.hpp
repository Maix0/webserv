/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rc.hpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:52:34 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/26 23:20:49 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <strings.h>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include "lib/Option.hpp"
#include "runtime/Logger.hpp"

#define assert_valid(OBJ) \
	assert((OBJ).ptr && ((OBJ).ptr->strong > 0 || ((OBJ).ptr->strong == 0 && (OBJ).ptr->weak > 0)))

#define assert_valid_strong(OBJ) assert((OBJ).ptr && ((OBJ).ptr->strong > 0))

template <typename T>
class Rc {
	private:
		struct RcInner {
				std::size_t strong;
				std::size_t weak;
				T*			value;
		};
		RcInner* ptr;

		Rc(RcInner* ptr) : ptr(ptr) { assert_valid(*this); };

	public:
		// Opaque type. Represent RcInner::RcInner
		struct RawRc;
		Rc() {
			this->ptr		  = new RcInner();
			this->ptr->strong = 1;
			this->ptr->weak	  = 0;
			// new (this->ptr->value.buffer()) T();
			this->ptr->value  = new T();
			assert_valid_strong(*this);
		}

		Rc(T* val) {
			if (val == NULL)
				throw std::runtime_error("Tried to init a shared ptr from NULL");
			this->ptr		  = new RcInner();
			this->ptr->strong = 1;
			this->ptr->weak	  = 0;
			this->ptr->value  = val;
			assert_valid_strong(*this);
		}

		~Rc() {
			assert_valid_strong(*this);
			--this->ptr->strong;
			if (this->ptr->strong == 0 && this->ptr->value != NULL) {
				delete this->ptr->value;
				this->ptr->value = NULL;
			}
			if (this->ptr->strong == 0 && this->ptr->weak == 0) {
				delete this->ptr;
			}
		}

		Rc(const Rc& rhs) {
			assert_valid_strong(rhs);
			this->ptr = rhs.ptr;
			if (this->ptr == NULL)
				throw std::runtime_error("shared.ptr == NULL");
			this->ptr->strong++;
		}
		Rc& operator=(const Rc& rhs) {
			assert_valid_strong(rhs);
			assert_valid_strong(*this);
			if (this != &rhs) {
				{
					Rc cpy = *this;
					// hello yes run the destructor pls
					this->ptr->strong--;
					(void)(cpy);
				}
				this->ptr = rhs.ptr;
				this->ptr->strong++;
			}
			return (*this);
		}

		T&		 operator*() { return (assert_valid_strong(*this), *this->ptr->value); }
		const T& operator*() const { return (assert_valid_strong(*this), *this->ptr->value); }
		T*		 operator->() { return (assert_valid_strong(*this), this->ptr->value); }
		const T* operator->() const { return (assert_valid_strong(*this), this->ptr->value); }

		// GetRaw
		RawRc* getRaw() {
			assert_valid_strong(*this);
			return ((RawRc*)this->ptr);
		}

		// GetRaw
		static Rc fromRaw(RawRc* raw) {
			if (raw == NULL)
				throw std::runtime_error("Tried to init a shared ptr from raw NULL");
			RcInner* raw_cast = (RcInner*)(raw);

			return Rc(raw_cast);
		}

		template <typename U>
		Rc<U> cast() {
			assert_valid_strong(*this);
			U* ptr = (this->ptr->value);
			assert((void*)ptr == (void*)this->ptr->value);
			this->ptr->strong++;
			RawRc* raw = this->getRaw();
			return Rc<U>::fromRaw((typename Rc<U>::RawRc*)raw);
		}

		template <typename U>
		Rc<U> try_cast() {
			assert_valid_strong(*this);
			T& val		= *this->ptr->value;
			U& val_cast = dynamic_cast<U&>(val);
			assert(&val == &val_cast);
			this->ptr->strong++;
			RawRc* raw = this->getRaw();
			return Rc<U>::fromRaw((typename Rc<U>::RawRc*)raw);
		}

		class Weak {
				RcInner* ptr;
				Weak(RcInner* ptr) : ptr(ptr) { assert_valid(*this); };

			public:
				static Weak make_weak(Rc& shared) {
					Weak out(reinterpret_cast<RcInner*>(shared.getRaw()));
					out.ptr->weak++;
					return out;
				};

				~Weak() {
					assert_valid(*this);
					--this->ptr->weak;
					if (this->ptr->strong == 0 && this->ptr->value != NULL) {
						delete this->ptr->value;
						this->ptr->value = NULL;
					}
					if (this->ptr->strong == 0 && this->ptr->weak == 0) {
						delete this->ptr;
					}
				}

				Weak() {
					this->ptr		  = new RcInner();
					this->ptr->value  = NULL;
					this->ptr->weak	  = 1;
					this->ptr->strong = 0;
					assert_valid(*this);
				}

				Weak(const Weak& rhs) {
					assert_valid(rhs);
					this->ptr = rhs.ptr;
					++this->ptr->weak;
				}
				Weak& operator=(const Weak& rhs) {
					assert_valid(*this);
					assert_valid(rhs);
					if (this != &rhs) {
						Weak cpy(*this);
						--this->ptr->weak;
						this->ptr = rhs.ptr;
						++this->ptr->weak;
						(void)(cpy);
					}
					return (*this);
				}

				Option<Rc> upgrade() {
					assert_valid(*this);
					if (this->ptr->strong != 0) {
						++this->ptr->strong;
						return Option<Rc>::Some(
							Rc(Rc::fromRaw(reinterpret_cast<RawRc*>(this->ptr))));
					}
					return Option<Rc>::None();
				}
		};

		static bool isSameRcPtr(const Rc& lhs, const Rc& rhs) { return lhs.ptr == rhs.ptr; };
		static bool isSameRcPtr(const Weak& lhs, const Rc& rhs) { return lhs.ptr == rhs.ptr; };
		static bool isSameRcPtr(const Rc& lhs, const Weak& rhs) { return lhs.ptr == rhs.ptr; };
		static bool isSameRcPtr(const Weak& lhs, const Weak& rhs) { return lhs.ptr == rhs.ptr; };
};

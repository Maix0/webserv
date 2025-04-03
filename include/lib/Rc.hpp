/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rc.hpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:52:34 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/02 17:56:49 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <strings.h>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include "lib/Option.hpp"


template <typename T>
class Rc {
	private:
		struct RcInner {
				std::size_t strong;
				std::size_t weak;
				T*			value;
		};
		RcInner* ptr;

		Rc(RcInner* ptr) : ptr(ptr) {
			if (this->ptr == NULL)
				throw std::runtime_error("shared.ptr == NULL");
		};

		inline void assert_valid() const {
			assert(this->ptr &&
				   (this->ptr->strong > 0 || (this->ptr->strong == 0 && this->ptr->weak > 0)));
		};

	public:
		// Opaque type. Represent RcInner::RcInner
		struct RawRc;
		Rc() {
			this->ptr		  = new RcInner();
			this->ptr->strong = 1;
			this->ptr->weak	  = 0;
			new (&this->ptr->value) T();
			this->assert_valid();
		}

		Rc(T* val) {
			if (val == NULL)
				throw std::runtime_error("Tried to init a shared ptr from NULL");
			this->ptr		  = new RcInner();
			this->ptr->strong = 1;
			this->ptr->weak	  = 0;
			this->ptr->value  = val;
			this->assert_valid();
		}

		~Rc() {
			this->assert_valid();
			if (this->ptr == NULL)
				throw std::runtime_error("shared.ptr == NULL");
			--this->ptr->strong;
			if (this->ptr->strong == 0) {
				delete this->ptr->value;
				this->ptr->value = NULL;
			}
			if (this->ptr->strong == 0 && this->ptr->weak == 0) {
				delete this->ptr;
			}
		}

		Rc(const Rc& rhs) {
			rhs.assert_valid();
			this->ptr = rhs.ptr;
			if (this->ptr == NULL)
				throw std::runtime_error("shared.ptr == NULL");
			this->ptr->strong++;
		}
		Rc& operator=(const Rc& rhs) {
			rhs.assert_valid();
			this->assert_valid();
			if (this != &rhs) {
				{
					this->ptr->strong--;
					Rc cpy = *this;
					// hello yes run the destructor pls
					(void)(cpy);
				}
				this->ptr = rhs.ptr;
				this->ptr->strong++;
			}
			return (*this);
		}

		T&		 operator*() { return (this->assert_valid(), *this->ptr->value); }
		const T& operator*() const { return (this->assert_valid(), *this->ptr->value); }
		T*		 operator->() { return (this->assert_valid(), this->ptr->value); }
		const T* operator->() const { return (this->assert_valid(), this->ptr->value); }

		// GetRaw
		RawRc* getRaw() {
			this->assert_valid();
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
			this->assert_valid();
			U* ptr = (this->ptr->value);
			assert((void*)ptr == (void*)this->ptr->value);
			this->ptr->strong++;
			RawRc* raw = this->getRaw();
			return Rc<U>::fromRaw((typename Rc<U>::RawRc*)raw);
		}

		template <typename U>
		Rc<U> try_cast() {
			this->assert_valid();
			T& val		= *this->ptr->value;
			U& val_cast = dynamic_cast<U&>(val);
			assert(&val == &val_cast);
			this->ptr->strong++;
			RawRc* raw = this->getRaw();
			return Rc<U>::fromRaw((typename Rc<U>::RawRc*)raw);
		}

		class Weak {
				RcInner* ptr;
				Weak(RcInner* ptr) : ptr(ptr) {
					if (this->ptr == NULL)
						throw std::runtime_error("weak.ptr == NULL");
				};

				inline void assert_valid() const {
					assert(this->ptr && (this->ptr->strong > 0 ||
										 (this->ptr->strong == 0 && this->ptr->weak > 0)));
				};

			public:
				static Weak make_weak(Rc& shared) {
					Weak out(static_cast<RcInner*>(shared.getRaw()));
					out.ptr->weak++;
					return out;
				};

				~Weak() {
					this->assert_valid();
					if (this->ptr == NULL)
						throw std::runtime_error("shared.ptr == NULL");
					--this->ptr->weak;
					if (this->ptr->strong == 0) {
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
					this->assert_valid();
				}

				Weak(const Weak& rhs) {
					rhs.assert_valid();
					this->ptr = rhs.ptr;
					++this->ptr->weak;
				}
				Weak& operator=(const Weak& rhs) {
					this->assert_valid();
					rhs.assert_valid();
					if (this != &rhs) {
						Weak cpy(*this);
						this->ptr = rhs.ptr;
						++this->ptr->weak;
						(void)(cpy);
					}
					return (*this);
				}

				Option<Rc> upgrade() {
					if (this->ptr->strong != 0) {
						++this->ptr->strong;
						return Option<Rc>::Some(
							new Rc(Rc::fromRaw(static_cast<RcInner*>(this->ptr))));
					}
					return Option<Rc>::None();
				}
		};

		static bool isSameRcPtr(const Rc& lhs, const Rc& rhs) { return lhs.ptr == rhs.ptr; };
		static bool isSameRcPtr(const Weak& lhs, const Rc& rhs) { return lhs.ptr == rhs.ptr; };
		static bool isSameRcPtr(const Rc& lhs, const Weak& rhs) { return lhs.ptr == rhs.ptr; };
		static bool isSameRcPtr(const Weak& lhs, const Weak& rhs) { return lhs.ptr == rhs.ptr; };
};

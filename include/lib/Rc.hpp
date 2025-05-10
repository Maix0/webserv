/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rc.hpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:52:34 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/07 08:54:12 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cassert>
#include <cstddef>
#include "lib/Functors.hpp"
#include "lib/Option.hpp"

struct FunctorMarker {};
#define RCFUNCTOR FunctorMarker()

template <typename T>
class Rc;

template <typename T>
class Weak;

template <typename T>
class RcBox {
	public:
		size_t strong_count;
		size_t weak_count;

		union {
				char		data[sizeof(T)];
				long double align;	// force max alignment for most types
		} storage;

		static T* getObject(RcBox* self) { return reinterpret_cast<T*>(self->storage.data); }

		template <typename Functor>
		static RcBox* create(Functor f) {
			RcBox* nptr		   = new RcBox();
			nptr->strong_count = 1;
			nptr->weak_count   = 1;
			try {
				f(RcBox::getObject(nptr));
			}
			catch (...) {
				delete nptr;
				throw;
			}
			return nptr;
		}

		static void destroyObject(RcBox* self) { RcBox::getObject(self)->~T(); }

	private:
		friend class Rc<T>;
		friend class Weak<T>;
};

template <typename T>
class Rc {
	public:
		Rc() : ptr(RcBox<T>::create(Functor0<T>())) {}

		template <typename Functor>
		Rc(Functor f, FunctorMarker functor_marker) : ptr(RcBox<T>::create(f)) {
			(void)(functor_marker);
		};

		Rc(const Rc& other) {
			this->ptr = other.ptr;
			if (this->ptr)
				this->ptr->strong_count++;
		}

		~Rc() { this->release(); }

		Rc& operator=(const Rc& other) {
			if (this != &other) {
				this->release();
				this->ptr = other.ptr;
				if (this->ptr)
					this->ptr->strong_count++;
			}
			return *this;
		}

		T* operator->() const { return RcBox<T>::getObject(this->ptr); }
		T& operator*() const { return *(RcBox<T>::getObject(this->ptr)); }

		static Rc fromRcBox(RcBox<T>* box) { return Rc(box, 0); }

		template <typename U>
		Rc<U> cast() {
			U* ptr = RcBox<T>::getObject(this->ptr);
			(void)(ptr);
			this->ptr->strong_count++;
			return Rc<U>::fromRcBox(reinterpret_cast<RcBox<U>*>(this->ptr));
		}

	private:
		RcBox<T>* ptr;
		void				 release() {
			if (this->ptr) {
				this->ptr->strong_count--;
				if (this->ptr->strong_count == 0) {
					RcBox<T>::destroyObject(this->ptr);
					this->ptr->weak_count--;
					if (this->ptr->weak_count == 0)
						delete this->ptr;
				}
				this->ptr = NULL;
			}
		}

		Rc(RcBox<T>* cb, int marker) : ptr(cb) { (void)(marker); }

		friend class Weak<T>;
};

template <typename T>
class Weak {
	public:
		Weak() : ptr(NULL) {}
		Weak(const Rc<T>& shared) {
			this->ptr = shared.ptr;
			if (this->ptr)
				this->ptr->weak_count++;
		}

		Weak(const Weak& other) {
			this->ptr = other.ptr;
			if (this->ptr)
				this->ptr->weak_count++;
		}

		~Weak() { this->release(); }

		Weak& operator=(const Weak& other) {
			if (this != &other) {
				this->release();
				this->ptr = other.ptr;
				if (this->ptr)
					this->ptr->weak_count++;
			}
			return *this;
		}

		Option<Rc<T> > upgrade() const {
			if (this->ptr != NULL && this->ptr->strong_count > 0) {
				this->ptr->strong_count++;
				return new Rc<T>(this->ptr, 0);
			}
			return Option<Rc<T> >::None();
		}

	private:
		RcBox<T>* ptr;

		void release() {
			if (this->ptr) {
				this->ptr->weak_count--;
				if (this->ptr->weak_count == 0 && this->ptr->strong_count == 0)
					delete this->ptr;
				ptr = NULL;
			}
		}

		friend class Rc<T>;
};

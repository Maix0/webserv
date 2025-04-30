/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rc.hpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:52:34 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/30 17:57:42 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/*
#include <strings.h>
#include <cassert>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include "lib/Functors.hpp"
#include "lib/Option.hpp"
#include "lib/aligned_storage.hpp"
#include "runtime/Logger.hpp"


void dbg();

#define OPERATOR_FIELD(NAME, FIELD, OPERATOR, TYPE, LIMIT)         \
	static void NAME##_##FIELD(_RcAlloc* self) {                   \
		if (self->f.FIELD > 10)                                    \
			dbg();                                                 \
		if (self->f.FIELD == std::numeric_limits<TYPE>::LIMIT()) { \
			return;                                                \
		}                                                          \
		self->f.FIELD OPERATOR;                                    \
	}

/// *
#define PRINT_RCBOX(rcbox)                        \
	if (rcbox != NULL)                            \
		LOG(debug, COL_GREEN << *rcbox << RESET); \
	else                                          \
		LOG(debug, COL_RED << "RcAlloc<T>(nil)" << RESET);
// * /
// #define PRINT_RCBOX(rcbox)

template <typename T>
struct _RcAlloc {
		// counts
		struct Data {
				unsigned int strong;
				unsigned int weak;
		};

		typename aligned_storage<sizeof(T), 16>::type v;
		Data										  f;

		static inline void assert_valid(_RcAlloc* self) {
			assert(self->f.strong > 0 || self->f.strong == 0 && self->f.weak > 0);
		}
		static T* getValuePtr(_RcAlloc* self) { return reinterpret_cast<T*>(&self->v); }

		static void destroy_value(_RcAlloc* self) {
			assert(self->f.strong == 0);
			reinterpret_cast<T*>(&self->v)->~T();
		}

		OPERATOR_FIELD(increase, strong, ++, unsigned int, max);
		OPERATOR_FIELD(decrease, strong, --, unsigned int, min);

		OPERATOR_FIELD(increase, weak, ++, unsigned int, max);
		OPERATOR_FIELD(decrease, weak, --, unsigned int, min);

		friend std::ostream& operator<<(std::ostream& o, const _RcAlloc& a) {
			o << "RcAlloc<T>(s: " << a.f.strong << ", w: " << a.f.weak << ")";
			return (o);
		}
};

#define PRIV_RETURN_VAL(TYPE, OBJ, PTR) \
	A::assert_valid((OBJ).alloc);       \
	return PTR(OBJ).getPtr();

struct FunctorMarker {};

template <typename T>
class Weak {
	private:
		typedef _RcAlloc<T> A;

		struct FromRawMarker {};

		A* alloc;
		Weak(A* alloc, FromRawMarker _marker) : alloc(alloc) {
			(void)(_marker);
			PRINT_RCBOX(alloc);
		};

	public:
		Weak() : alloc(NULL) { PRINT_RCBOX(alloc); }
		~Weak() {
			PRINT_RCBOX(this->alloc);
			if (this->alloc == NULL)
				return;
			A::decrease_weak(this->alloc);
			if (this->alloc->f.strong == 0 && this->alloc->f.weak == 0)
				delete this->alloc;
		}

		Weak(const Weak& rhs) : alloc(NULL) { *this = rhs; }
		Weak& operator=(const Weak& rhs) {
			PRINT_RCBOX(alloc);
			if (this == &rhs)
				return (*this);
			this->~Weak();
			this->alloc = rhs.alloc;
			if (this->alloc != NULL)
				A::increase_weak(this->alloc);
			return (*this);
		}

		static _RcAlloc<T>* _getAlloc(const Weak& self) { return self->alloc; };

		static Weak<T> fromRaw(T* raw) {
			assert(raw != NULL);
			A* alloc_ptr = (A*)((char*)raw - offsetof(A, v));
			return Weak(alloc_ptr, FromRawMarker());
		}
};

template <typename T>
class Rc {
	private:
		typedef _RcAlloc<T> A;

		struct FromRawMarker {};

		_RcAlloc<T>* alloc;
		Rc(T* ptr, FromRawMarker _marker) : alloc(NULL) {
			A* alloc_ptr = ;
			this->alloc	 = alloc_ptr;
			(void)(_marker);
		};

		T*		 getPtr() { return A::getValuePtr(this->alloc); };
		const T* getPtr() const { return A::getValuePtr(this->alloc); };

	public:
		static Option<Rc> upgrade(const Weak<T>& weak) {
			A* raw = Weak<T>::_getAlloc(weak);
			PRINT_RCBOX(raw);
			if (raw == NULL || raw->f.strong == 0)
				return Option<Rc>::None();

			A::increase_strong(raw);
			return Rc(A::getValuePtr(raw), FromRawMarker());
		}

		static Weak<T> downgrade(const Rc& self) {
			PRINT_RCBOX(self.alloc);
			A::increase_weak(self.alloc);
			return Weak<T>::fromRaw(A::getValuePtr(self.alloc));
		}

		Rc() {
			this->alloc			  = new _RcAlloc<T>();
			this->alloc->f.strong = 1;
			this->alloc->f.weak	  = 1;
			Functor0<T>()(A::getValuePtr(this->alloc));
			PRINT_RCBOX(this->alloc);
		}

		template <typename F>
		Rc(F constructor, FunctorMarker marker) {
			(void)marker;
			this->alloc			  = new _RcAlloc<T>();
			this->alloc->f.strong = 1;
			this->alloc->f.weak	  = 1;
			constructor(A::getValuePtr(this->alloc));
			PRINT_RCBOX(this->alloc);
		}
		~Rc() {
			PRINT_RCBOX(this->alloc);
			if (this->alloc == NULL)
				return;
			A::decrease_strong(this->alloc);
			if (this->alloc->f.strong == 0) {
				A::destroy_value(this->alloc);
				A::decrease_weak(this->alloc);
			}
			if (this->alloc->f.strong == 0 && this->alloc->f.weak == 0)
				delete this->alloc;
		}

		Rc(const Rc& rhs) : alloc(NULL) { *this = rhs; }
		Rc& operator=(const Rc& rhs) {
			if (this == &rhs)
				return (*this);
			this->~Rc();
			this->alloc = rhs.alloc;
			A::increase_strong(this->alloc);
			PRINT_RCBOX(this->alloc);
			return (*this);
		}

		T&		 operator*() { PRIV_RETURN_VAL(T, *this, *); }
		const T& operator*() const { PRIV_RETURN_VAL(T, *this, *); }
		T*		 operator->() { PRIV_RETURN_VAL(T, *this, ); }
		const T* operator->() const { PRIV_RETURN_VAL(T, *this, ); }

		// GetRaw
		T* getRaw() {
			A::assert_valid(this->alloc);
			return this->getPtr();
		}

		// GetRaw
		static Rc fromRaw(T* raw) {
			assert(raw != NULL);
			return Rc(raw, FromRawMarker());
		}

		template <typename U>
		Rc<U> cast() {
			U* ptr = this->getPtr();
			A::increase_strong(this->alloc);
			return Rc<U>::fromRaw(ptr);
		}

		template <typename U>
		Rc<U> try_cast() {
			U* ptr = dynamic_cast<U*>(this->getPtr());
			A::increase_strong(this->alloc);
			return Rc<U>::fromRaw(ptr);
		}

		static void increase_strong(const Rc& self) { A::increase_strong(self->alloc); }
		static void decrease_strong(const Rc& self) { A::decrease_strong(self->alloc); }
		static void increase_weak(const Rc& self) { A::increase_weak(self->alloc); }
		static void decrease_weak(const Rc& self) { A::decrease_weak(self->alloc); }

		static unsigned int strong_count(const Rc& self) { return self.alloc->f.strong; };
		static unsigned int weak_count(const Rc& self) { return self.alloc->f.weak; };
};
*/

#include <cassert>
#include <cstddef>
#include <new>
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
			RcBox* cb		 = new RcBox();
			cb->strong_count = 1;
			cb->weak_count	 = 1;
			f(RcBox::getObject(cb));
			return cb;
		}

		static void destroyObject(RcBox* self) { RcBox::getObject(self)->~T(); }
		static void deleteSelf(RcBox* self) { delete self; }

	private:
		friend class Rc<T>;
		friend class Weak<T>;
};

template <typename T>
class Rc {
	public:
		Rc() : cb(RcBox<T>::create(Functor0<T>())) {}

		template <typename Functor>
		Rc(Functor f, FunctorMarker functor_marker) : cb(RcBox<T>::create(f)) {
			(void)(functor_marker);
		};

		Rc(const Rc& other) {
			this->cb = other.cb;
			if (this->cb)
				this->cb->strong_count++;
		}

		~Rc() { this->release(); }

		Rc& operator=(const Rc& other) {
			if (this != &other) {
				this->release();
				this->cb = other.cb;
				if (this->cb)
					this->cb->strong_count++;
			}
			return *this;
		}

		T* operator->() const { return RcBox<T>::getObject(this->cb); }
		T& operator*() const { return *(RcBox<T>::getObject(this->cb)); }

		static Rc fromRcBox(RcBox<T>* box) { return Rc(box, 0); }
		static Rc fromRaw(T* raw) {
			RcBox<T>* box = (RcBox<T>*)((char*)raw - offsetof(RcBox<T>, storage.data));
			box->strong_count++;
			return Rc(box, 0);
		}

		template <typename U>
		Rc<U> cast() {
			U* ptr = RcBox<T>::getObject(this->cb);
			(void)(ptr);
			this->cb->strong_count++;
			return Rc<U>::fromRcBox(reinterpret_cast<RcBox<U>*>(this->cb));
		}

	private:
		RcBox<T>* cb;
		void	  release() {
			 if (this->cb) {
				 if (--this->cb->strong_count == 0) {
					 RcBox<T>::destroyObject(this->cb);
					 if (--this->cb->weak_count == 0)
						 RcBox<T>::deleteSelf(this->cb);
				 }
				 this->cb = NULL;
			 }
		}

		Rc(RcBox<T>* cb, int marker) : cb(cb) { (void)(marker); }

		friend class Weak<T>;
};

template <typename T>
class Weak {
	public:
		Weak() : cb(NULL) {}
		Weak(const Rc<T>& shared) {
			this->cb = shared.cb;
			if (this->cb)
				this->cb->weak_count++;
		}

		Weak(const Weak& other) {
			this->cb = other.cb;
			if (this->cb)
				this->cb->weak_count++;
		}

		~Weak() { this->release(); }

		Weak& operator=(const Weak& other) {
			if (this != &other) {
				this->release();
				this->cb = other.cb;
				if (this->cb)
					this->cb->weak_count++;
			}
			return *this;
		}

		Option<Rc<T> > upgrade() const {
			if (this->cb != NULL && this->cb->strong_count > 0) {
				this->cb->strong_count++;
				return Rc<T>(this->cb, 0);
			}
			return Option<Rc<T> >::None();
		}

	private:
		RcBox<T>* cb;

		void release() {
			if (this->cb) {
				if (--this->cb->weak_count == 0 && this->cb->strong_count == 0) {
					RcBox<T>::deleteSelf(this->cb);
				}
				cb = NULL;
			}
		}

		friend class Rc<T>;
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Functors.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 19:48:30 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/28 20:01:33 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

template <typename T,
		  typename A,
		  typename B,
		  typename C,
		  typename D,
		  typename E,
		  typename F,
		  typename G,
		  typename H,
		  typename I,
		  typename J>
struct Functor10 {
		A a;
		B b;
		C c;
		D d;
		E e;
		F f;
		G g;
		H h;
		I i;
		J j;

		Functor10(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j)
			: a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h), i(i), j(j) {}
		~Functor10() {}

		void operator()(T* ptr) { new (ptr) T(a, b, c, d, e, f, g, h, i, j); }
};

template <typename T,
		  typename A,
		  typename B,
		  typename C,
		  typename D,
		  typename E,
		  typename F,
		  typename G,
		  typename H,
		  typename I>
struct Functor9 {
		A a;
		B b;
		C c;
		D d;
		E e;
		F f;
		G g;
		H h;
		I i;

		Functor9(A a, B b, C c, D d, E e, F f, G g, H h, I i)
			: a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h), i(i) {}
		~Functor9() {}

		void operator()(T* ptr) { new (ptr) T(a, b, c, d, e, f, g, h, i); }
};

template <typename T,
		  typename A,
		  typename B,
		  typename C,
		  typename D,
		  typename E,
		  typename F,
		  typename G,
		  typename H>
struct Functor8 {
		A a;
		B b;
		C c;
		D d;
		E e;
		F f;
		G g;
		H h;

		Functor8(A a, B b, C c, D d, E e, F f, G g, H h)
			: a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h) {}
		~Functor8() {}

		void operator()(T* ptr) { new (ptr) T(a, b, c, d, e, f, g, h); }
};

template <typename T,
		  typename A,
		  typename B,
		  typename C,
		  typename D,
		  typename E,
		  typename F,
		  typename G>
struct Functor7 {
		A a;
		B b;
		C c;
		D d;
		E e;
		F f;
		G g;

		Functor7(A a, B b, C c, D d, E e, F f, G g) : a(a), b(b), c(c), d(d), e(e), f(f), g(g) {}
		~Functor7() {}

		void operator()(T* ptr) { new (ptr) T(a, b, c, d, e, f, g); }
};

template <typename T, typename A, typename B, typename C, typename D, typename E, typename F>
struct Functor6 {
		A a;
		B b;
		C c;
		D d;
		E e;
		F f;

		Functor6(A a, B b, C c, D d, E e, F f) : a(a), b(b), c(c), d(d), e(e), f(f) {}
		~Functor6() {}

		void operator()(T* ptr) { new (ptr) T(a, b, c, d, e, f); }
};

template <typename T, typename A, typename B, typename C, typename D, typename E>
struct Functor5 {
		A a;
		B b;
		C c;
		D d;
		E e;

		Functor5(A a, B b, C c, D d, E e) : a(a), b(b), c(c), d(d), e(e) {}
		~Functor5() {}

		void operator()(T* ptr) { new (ptr) T(a, b, c, d, e); }
};

template <typename T, typename A, typename B, typename C, typename D>
struct Functor4 {
		A a;
		B b;
		C c;
		D d;

		Functor4(A a, B b, C c, D d) : a(a), b(b), c(c), d(d) {}
		~Functor4() {}

		void operator()(T* ptr) { new (ptr) T(a, b, c, d); }
};

template <typename T, typename A, typename B, typename C>
struct Functor3 {
		A a;
		B b;
		C c;

		Functor3(A a, B b, C c) : a(a), b(b), c(c) {}
		~Functor3() {}

		void operator()(T* ptr) { new (ptr) T(a, b, c); }
};

template <typename T, typename A, typename B>
struct Functor2 {
		A a;
		B b;

		Functor2(A a, B b) : a(a), b(b) {}
		~Functor2() {}

		void operator()(T* ptr) { new (ptr) T(a, b); }
};

template <typename T, typename A>
struct Functor1 {
		A a;

		Functor1(A a) : a(a) {}
		~Functor1() {}

		void operator()(T* ptr) { new (ptr) T(a); }
};

template <typename T>
struct Functor0 {
		Functor0() {}
		~Functor0() {}

		void operator()(T* ptr) { new (ptr) T(); }
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aligned_storage.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 02:02:35 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/27 02:55:57 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stddef.h>

#define ALIGNED_STORAGE_DEF(ALIGN, ALIGNER_TYPE)  \
	template <size_t SIZE>                        \
	struct aligned_storage<SIZE, (ALIGN)> {       \
			static const size_t _ALIGN = (ALIGN); \
			static const size_t _SIZE  = SIZE;    \
                                                  \
			union type {                          \
					ALIGNER_TYPE _align;          \
					char		 data[SIZE];      \
			};                                    \
	}

template <size_t SIZE, size_t ALIGN>
struct aligned_storage;

ALIGNED_STORAGE_DEF(1 << 0, char);
ALIGNED_STORAGE_DEF(1 << 1, short);
ALIGNED_STORAGE_DEF(1 << 2, int);
ALIGNED_STORAGE_DEF(1 << 3, long);
ALIGNED_STORAGE_DEF(1 << 4, long double);

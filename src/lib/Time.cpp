/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Time.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 19:30:11 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 19:34:47 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib/Time.hpp"
#include "runtime/Logger.hpp"

#include <cassert>
#include <ctime>

Time Time::now() {
	Time out;

	_ERR_RET_THROW(out.inner = time(NULL));
	return out;
}

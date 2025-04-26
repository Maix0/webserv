/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:59:36 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/26 21:44:23 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib/Rc.hpp"
#include "runtime/EpollType.hpp"

class Epoll;
// A callback function that handles an epoll event
class Callback {
	public:
		Callback() {};
		virtual ~Callback() {};
		virtual void	  call(Epoll& epoll, Rc<Callback> self) = 0;
		virtual int		  getFd()								= 0;
		virtual EpollType getTy()								= 0;
};

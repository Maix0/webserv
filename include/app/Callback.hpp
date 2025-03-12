/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:59:36 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 14:46:05 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace app {
	struct EpollEvent;
	class Epoll;

	// A callback function that handles an epoll event
	class Callback {
	public:
		virtual ~Callback() {};
		virtual void call(Epoll& epoll, EpollEvent event) = 0;
	};
}  // namespace app

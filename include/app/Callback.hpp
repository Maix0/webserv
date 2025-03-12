/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:59:36 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/12 18:18:07 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "app/Shared.hpp"
namespace app {
	struct EpollEvent;
	class Epoll;

	// A callback function that handles an epoll event
	class Callback {
	public:
		virtual ~Callback() {};
		virtual void call(Epoll& epoll, app::Shared<Callback> self) = 0;
	};
}  // namespace app

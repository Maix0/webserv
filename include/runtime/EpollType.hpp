/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollType.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 16:16:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/13 16:16:28 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/epoll.h>

enum EpollType {
	READ   = EPOLLIN,
	WRITE  = EPOLLOUT,
	HANGUP = EPOLLHUP | EPOLLRDHUP
};

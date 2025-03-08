/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:01:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/08 18:24:26 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include "app/Shared.hpp"
#include "app/Socket.hpp"

namespace app {
	class Epoll {
	private:
		// no copy for you !
		Epoll(const Epoll&);
		Epoll&					   operator=(const Epoll&);

		int						   fd;
		std::vector<Shared<AsFd> > watched;

	public:
		Epoll();
		~Epoll();

		void							  addFd(Shared<AsFd> fd);
		void							  removeFd(Shared<AsFd> fd);

		const std::vector<Shared<AsFd> >& getWatched() const { return (this->watched); }
	};
};	// namespace app

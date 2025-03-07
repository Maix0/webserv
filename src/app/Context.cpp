/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:14:09 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:57:55 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Context.hpp"
#include "app/Logger.hpp"

namespace app {

	Context Context::INSTANCE = Context();

	Context::Context() {
		LOG(trace, "creating context singleton");
	}

	Context::~Context() {
		LOG(trace, "destroying context singleton");
	}

	Context& Context::getInstance() {
		return (Context::INSTANCE);
	}
};	// namespace app

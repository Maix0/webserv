/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 22:14:09 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/02 22:28:54 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Context.hpp"
#include "app/Logger.hpp"

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

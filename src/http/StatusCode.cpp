/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusCode.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:57:00 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 13:57:21 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/http/StatusCode.hpp"
#include <string>

Option<std::string> StatusCode::canonical() const {
	try {
		return Option<std::string>::Some(status::_NAMES.at(this->_code));
	} catch (...) {
		return Option<std::string>::None();
	}
}

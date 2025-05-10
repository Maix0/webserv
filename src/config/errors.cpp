/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 21:04:29 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:40:30 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/Config.hpp"

namespace config {
#define IMPL_ERROR(NAME, MSG)                                                            \
	NAME##Error::NAME##Error() : msg(MSG){}                                             \
	NAME##Error::NAME##Error(const std::string& m) : msg(std::string(MSG) + ": " + m){} \
	NAME##Error::NAME##Error(const NAME##Error& rhs) : msg(rhs.msg){}                   \
	NAME##Error::~NAME##Error() throw(){}                                               \
                                                                                         \
	NAME##Error& NAME##Error::operator=(const NAME##Error& rhs) {                        \
		this->msg = rhs.msg;                                                             \
		return (*this);                                                                  \
	}                                                                                   \
	const char* NAME##Error::what() const throw() {                                      \
		return (this->msg.c_str());                                                      \
	}

	IMPL_ERROR(ConfigParse, "config parsing error")
	IMPL_ERROR(CgiParse, "cgi parsing error")
	IMPL_ERROR(ServerParse, "server parsing error")
	IMPL_ERROR(RouteParse, "route parsing error")
}  // namespace config

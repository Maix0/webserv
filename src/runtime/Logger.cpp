/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 19:35:46 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/07 08:56:49 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "runtime/Logger.hpp"

namespace log {
	LogLevel	logLevel = ::log::_compileTimeLogLevel();
	const char* logPidColor = COL_CYAN;
}  // namespace log

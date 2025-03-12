/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shim.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:23:58 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:43:27 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// YOU SHOULD ALMOST NEVER TOUCH THIS FILE !

#include <exception>
#include "app/Logger.hpp"

::log::LogLevel(::log::logLevel) = ::log::_compileTimeLogLevel();

int wrapped_main(char* argv0, int argc, char* argv[], char* envp[]);

int main(int argc, char* argv[], char* envp[]) {
	try {
		char* argv0 = *argv;
		(void)(argc--, argv++);
		return wrapped_main(argv0, argc, argv, envp);
	} catch (const std::exception& e) {
		LOG(fatal, e.what());
	}
	return 1;
}

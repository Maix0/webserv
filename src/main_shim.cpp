/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_shim.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:23:58 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/05 21:40:12 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include "app/Logger.hpp"

namespace log {

LogLevel logLevel = NONE;

}

int wrapped_main(int argc, char* argv[], char* envp[]);

int main(int argc, char* argv[], char* envp[]) {
	log::_setLogCompileTimeLogLevel();
	log::_setEnvLogLevel(envp);
	try {
		return wrapped_main(argc, argv, envp);
	} catch (const std::exception& e) {
		LOG(fatal, e.what());
	}
	return 1;
}

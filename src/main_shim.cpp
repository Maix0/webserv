/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_shim.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:23:58 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/05 22:17:31 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include "app/Logger.hpp"

namespace log {

LogLevel logLevel = _compileTimeLogLevel();

}

int wrapped_main(int argc, char* argv[], char* envp[]);

int main(int argc, char* argv[], char* envp[]) {
	try {
		return wrapped_main(argc, argv, envp);
	} catch (const std::exception& e) {
		LOG(fatal, e.what());
	}
	return 1;
}

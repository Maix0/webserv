/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shim.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:23:58 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 19:35:39 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// YOU SHOULD ALMOST NEVER TOUCH THIS FILE !

#include <cstring>
#include <exception>
#include <string>
#include "runtime/Logger.hpp"

int wrapped_main(char* argv0, int argc, char* argv[], char* envp[]);

extern char __flags_start[];
extern char __flags_end[];
extern char __flags_size[];

int main(int argc, char* argv[], char* envp[]) {
	if (argc >= 2 && strncmp(argv[1], "--print-flags", 15) == 0) {
		try {
			std::string msg(__flags_start, __flags_end);
			std::cout << msg;
			std::cout.flush();
		} catch (...) {
		}
		return 0;
	}
	try {
		char* argv0 = *argv;
		(void)(argc--, argv++);
		return wrapped_main(argv0, argc, argv, envp);
	} catch (const std::exception& e) {
		LOG(fatal, e.what());
	}
	return 1;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shim.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:23:58 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/08 16:48:59 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// YOU SHOULD ALMOST NEVER TOUCH THIS FILE !

#include <cstdlib>
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
	char* argv0 = *argv;
	(void)(argc--, argv++);
	return wrapped_main(argv0, argc, argv, envp);
}

#define TERMINATE_BONUS

#ifdef TERMINATE_BONUS

#	include <execinfo.h>
#	include <string.h>
#	include <dlfcn.h>
#	include <link.h>

#	include <cstdlib>
#	include <cstdio>
#	include <iostream>

#	ifndef BACKTRACE_DEEP
#		define BACKTRACE_DEEP 50
#	endif

#	ifndef BASE_PATH
#		define BASE_PATH ""
#	endif

static size_t convert_to_vma(size_t addr) {
	Dl_info			 info;
	struct link_map* link_map;

	dladdr1((void*)addr, &info, (void**)&link_map, RTLD_DL_LINKMAP);
	return (addr - link_map->l_addr);
}

static void print_trace_inner(void** trace, char** messages, size_t i) {
	char syscom[1024];
	int	 p;

	p = 0;
	fprintf(stderr, "[bt] #%-4zu\t", i);
	while (messages[i][p] != '(' && messages[i][p] != ' ' && messages[i][p] != 0)
		++p;
	fflush(stderr);
	snprintf(syscom, sizeof(syscom) / sizeof(syscom[0]),
			 "addr2line %#x -e %.*s -ipf"
			 "| 1>&2 rg \"^(.*) at %s(.*)\"'$' --replace '$1 at $2' --color never",
			 (unsigned int)(convert_to_vma((size_t)trace[i])), p, messages[i], BASE_PATH);
	if (system(syscom))
		fprintf(stderr, "%s\n", messages[i]);
}

void print_trace(void) {
	void*  trace[BACKTRACE_DEEP];
	char** messages;
	int	   size;
	int	   i;

	size	 = backtrace(trace, BACKTRACE_DEEP);
	messages = backtrace_symbols(trace, size);
	i		 = 1;
	fprintf(stderr, "[bt] Execution path:\n");
	if (size >= 3)
		size -= 3;
	while (i < size)
		print_trace_inner(trace, messages, i++);
	free(messages);
}
#else
void print_trace(void) {};
#endif

void my_terminate() {
	static int tried_throw = 0;

	try {
		// try once to re-throw currently active exception
		if (!tried_throw++)
			throw;
	} catch (const std::exception& e) {
		LOG(fatal, " caught unhandled exception. what(): " << e.what());
	} catch (...) {
		LOG(fatal, " caught unknown/unhandled exception");
	}
	print_trace();
	std::exit(EXIT_FAILURE);
}

namespace {
	// invoke set_terminate as part of global constant initialization
	static const bool SET_TERMINATE = std::set_terminate(my_terminate);
}  // namespace

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:29:43 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/02 22:24:23 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#define RESET		   "\x1b[0m"
#define BOLD		   "\x1b[1m"
#define WEAK		   "\x1b[2m"
#define ITALIC		   "\x1b[3m"
#define UNDERLINE	   "\x1b[4m"

#define COL_BLACK	   "\x1b[30m"
#define COL_RED		   "\x1b[31m"
#define COL_GREEN	   "\x1b[32m"
#define COL_YELLOW	   "\x1b[33m"
#define COL_BLUE	   "\x1b[34m"
#define COL_MAGENTA	   "\x1b[35m"
#define COL_CYAN	   "\x1b[36m"
#define COL_WHITE	   "\x1b[37m"

#define COLB_BLACK	   "\x1b[90m"
#define COLB_RED	   "\x1b[91m"
#define COLB_GREEN	   "\x1b[92m"
#define COLB_YELLOW	   "\x1b[93m"
#define COLB_BLUE	   "\x1b[94m"
#define COLB_MAGENTA   "\x1b[95m"
#define COLB_CYAN	   "\x1b[96m"
#define COLB_WHITE	   "\x1b[97m"

#define COLBG_BLACK	   "\x1b[40m"
#define COLBG_RED	   "\x1b[41m"
#define COLBG_GREEN	   "\x1b[42m"
#define COLBG_YELLOW   "\x1b[43m"
#define COLBG_BLUE	   "\x1b[44m"
#define COLBG_MAGENTA  "\x1b[45m"
#define COLBG_CYAN	   "\x1b[46m"
#define COLBG_WHITE	   "\x1b[47m"

#define COLBBG_BLACK   "\x1b[10m0"
#define COLBBG_RED	   "\x1b[10m1"
#define COLBBG_GREEN   "\x1b[10m2"
#define COLBBG_YELLOW  "\x1b[10m3"
#define COLBBG_BLUE	   "\x1b[10m4"
#define COLBBG_MAGENTA "\x1b[10m5"
#define COLBBG_CYAN	   "\x1b[10m6"
#define COLBBG_WHITE   "\x1b[10m7"

#define none		   -1
#define fatal		   0
#define err			   1
#define warn		   2
#define info		   3
#define debug		   4
#define trace		   5

#ifndef LEVEL
#	define LEVEL info
#endif

#define HEADER_fatal	   "[" COL_RED UNDERLINE BOLD "FATAL" RESET "]"
#define HEADER_err		   "[" COL_RED BOLD "ERROR" RESET "]"
#define HEADER_warn		   "[" COLB_YELLOW BOLD "WARN " RESET "]"
#define HEADER_info		   "[" COL_BLUE BOLD "INFO " RESET "]"
#define HEADER_debug	   "[" COL_MAGENTA BOLD "DEBUG" RESET "]"
#define HEADER_trace	   "[" COLB_WHITE BOLD "TRACE" RESET "]"

#define FILTER_fatal(code) code
#define FILTER_err(code)   code
#define FILTER_warn(code)  code
#define FILTER_info(code)  code
#define FILTER_debug(code) code
#define FILTER_trace(code) code

#if LEVEL == none
#	undef FILTER_fatal
#	undef FILTER_err
#	undef FILTER_warn
#	undef FILTER_info
#	undef FILTER_debug
#	undef FILTER_trace

#	define FILTER_fatal(code)
#	define FILTER_err(code)
#	define FILTER_warn(code)
#	define FILTER_info(code)
#	define FILTER_debug(code)
#	define FILTER_trace(code)
#elif LEVEL == fatal
#	undef FILTER_err
#	undef FILTER_warn
#	undef FILTER_info
#	undef FILTER_debug
#	undef FILTER_trace

#	define FILTER_err(code)
#	define FILTER_warn(code)
#	define FILTER_info(code)
#	define FILTER_debug(code)
#	define FILTER_trace(code)
#elif LEVEL == err
#	undef FILTER_warn
#	undef FILTER_info
#	undef FILTER_debug
#	undef FILTER_trace

#	define FILTER_warn(code)
#	define FILTER_info(code)
#	define FILTER_debug(code)
#	define FILTER_trace(code)
#elif LEVEL == warn
#	undef FILTER_info
#	undef FILTER_debug
#	undef FILTER_trace

#	define FILTER_info(code)
#	define FILTER_debug(code)
#	define FILTER_trace(code)
#elif LEVEL == info
#	undef FILTER_debug
#	undef FILTER_trace

#	define FILTER_debug(code)
#	define FILTER_trace(code)
#elif LEVEL == debug
#	undef FILTER_trace

#	define FILTER_trace(code)
#elif LEVEL == trace
#else
#	warning "Invalid LEVEL" LEVEL
#endif

#undef fatal
#undef err
#undef warn
#undef info
#undef debug
#undef trace

#undef LEVEL

#define LOG(level, code)                                                                        \
	do {                                                                                        \
		FILTER_##level(std::cerr << HEADER_##level << " " << __FUNCTION__ << " in " << __FILE__ \
								 << ":" << __LINE__ << " " << code << std::endl;);              \
	} while (0)

// LOG(info, banane << "code" << "truc");

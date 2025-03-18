/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:29:43 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/15 09:56:49 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <string>

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

#ifndef LOG_LEVEL
#	define LOG_LEVEL info
#endif

/// You shouldn't use this namespace :)
namespace log {
	enum LogLevel {
		NONE  = none,
		FATAL = fatal,
		ERR	  = err,
		WARN  = warn,
		INFO  = info,
		DEBUG = debug,
		TRACE = trace,
	};

	extern LogLevel logLevel;

	static inline bool _setEnvLogLevel(char** envp) {
		try {
			while (*envp) {
				std::string			   env = *envp;
				std::string::size_type eq  = env.find_first_of('=');
				std::string			   key = env.substr(0, eq);
				if (key == "LOG_LEVEL") {
					static std::pair<std::string, LogLevel> levels[] = {
						// clang-format off
					std::make_pair("none", NONE),
					std::make_pair("fatal", FATAL),
					std::make_pair("err", ERR),
					std::make_pair("warn", WARN),
					std::make_pair("info", INFO),
					std::make_pair("debug", DEBUG),
					std::make_pair("trace", TRACE),

					// aliases
					std::make_pair("error", ERR),
					std::make_pair("warning", WARN),
					// maybe should this be set to trace ?
					std::make_pair("", NONE),
						// clang-format on
					};

					std::string val = env.substr(eq + 1);

					// poor man's string.toLowercase()...
					for (std::string::iterator it = val.begin(); it != val.end(); it++)
						*it = std::tolower(*it);

					for (std::size_t i = 0; i < sizeof(levels) / sizeof(levels[0]); i++) {
						if (levels[i].first == val) {
							logLevel = levels[i].second;
							return true;
						}
					}
				}
				envp++;
			}
		} catch (const ::std::exception& e) {
			std::cerr << "EARLY FATAL: " << e.what() << std::endl;
			std::exit(1);
		}
		return false;
	}

	static inline bool _shouldLog(LogLevel level) {
		return (level <= logLevel);
	}

	static inline LogLevel _compileTimeLogLevel(void) {
		if (log::_setEnvLogLevel(environ))
			return logLevel;
		if (LOG_LEVEL < none || LOG_LEVEL > trace)
			return (TRACE);
		return (LogLevel)LOG_LEVEL;
	}

	static inline void setLogLevel(LogLevel level) {
		logLevel = level;
	}

}  // namespace log

#define HEADER_fatal "[" COL_RED UNDERLINE BOLD "FATAL" RESET "]"
#define HEADER_err	 "[" COL_RED BOLD "ERROR" RESET "]"
#define HEADER_warn	 "[" COLB_YELLOW BOLD "WARN " RESET "]"
#define HEADER_info	 "[" COL_BLUE BOLD "INFO " RESET "]"
#define HEADER_debug "[" COL_MAGENTA BOLD "DEBUG" RESET "]"
#define HEADER_trace "[" COLB_WHITE BOLD "TRACE" RESET "]"

#ifndef LOG_DISABLE
// clang-format off
#define FILTER_fatal(code) if (::log::_shouldLog(::log::FATAL)) {code}
#define FILTER_err(code)   if (::log::_shouldLog(::log::ERR))   {code}
#define FILTER_warn(code)  if (::log::_shouldLog(::log::WARN))  {code}
#define FILTER_info(code)  if (::log::_shouldLog(::log::INFO))  {code}
#define FILTER_debug(code) if (::log::_shouldLog(::log::DEBUG)) {code}
#define FILTER_trace(code) if (::log::_shouldLog(::log::TRACE)) {code}
// clang-format on
#else
// clang-format off
#define FILTER_fatal(code)
#define FILTER_err(code)
#define FILTER_warn(code)
#define FILTER_info(code)
#define FILTER_debug(code)
#define FILTER_trace(code)
// clang-format on
#endif

#undef fatal
#undef err
#undef warn
#undef info
#undef debug
#undef trace

#define LOG(level, code)                                                                        \
	do {                                                                                        \
		FILTER_##level(std::cerr << HEADER_##level << " " << __FUNCTION__ << " in " << __FILE__ \
								 << ":" << __LINE__ << " " << code << std::endl;);              \
	} while (0)

#include <cerrno>
#include <cstring>

#define _ERR_RET(code)                                       \
	if ((code) < 0) {                                        \
		int serr = errno;                                    \
		(void)serr;                                          \
		LOG(debug, "early return here: " << strerror(serr)); \
		return;                                              \
	}

#define _ERR_RET_THROW(code)                                 \
	if ((code) < 0) {                                        \
		int serr = errno;                                    \
		(void)serr;                                          \
		LOG(debug, "early return here: " << strerror(serr)); \
		throw std::runtime_error("check failed");            \
	}

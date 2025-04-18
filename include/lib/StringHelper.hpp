/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringHelper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 13:44:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/18 13:57:34 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

// stolen from stack overflow: https://stackoverflow.com/a/25385766
static const char* ws = " \t\n\r\f\v";

// trim from end of string (right)
static inline std::string& string_rtrim(std::string& s, const char* t = ws) {
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

// trim from beginning of string (left)
static inline std::string& string_ltrim(std::string& s, const char* t = ws) {
	s.erase(0, s.find_first_not_of(t));
	return s;
}

// trim from both ends of string (right then left)
static inline std::string& string_trim(std::string& s, const char* t = ws) {
	return string_ltrim(string_rtrim(s, t), t);
}

static inline bool string_start_with(const std::string& self, const std::string& prefix) {
	std::string::const_iterator sit = self.begin(), pit = prefix.begin();
	for (; sit != self.end() && pit != prefix.end(); sit++, pit++)
		if (*sit != *pit)
			return false;

	return pit == prefix.end();
}

#define _HTML_ESCAPE(raw, code) \
	case raw: {                 \
		out += "&#" #code ";";  \
		break;                  \
	}

static inline std::string string_escape_html(const std::string& self) {
	std::string out;
	out.reserve(self.capacity());
	for (std::string::const_iterator chr = self.begin(); chr != self.end(); chr++) {
		switch (*chr) {
			_HTML_ESCAPE('\t', 9);
			_HTML_ESCAPE('\n', 10);
			_HTML_ESCAPE(' ', 32);
			_HTML_ESCAPE('"', 34);
			_HTML_ESCAPE('&', 38);
			_HTML_ESCAPE('<', 60);
			_HTML_ESCAPE('>', 62);
			default: {
				out.push_back(*chr);
				break;
			}
		}
	}
	return out;
}

static inline std::string& string_tolower(std::string& s) {
	for (std::string::iterator it = s.begin(); it != s.end(); it++)
		if ('A' <= *it && *it <= 'Z')
			*it += 'a' - 'A';
	return s;
}

static inline std::string& string_toupper(std::string& s) {
	for (std::string::iterator it = s.begin(); it != s.end(); it++)
		if ('a' <= *it && *it <= 'z')
			*it -= 'a' - 'A';
	return s;
}

#undef _HTML_ESCAPE

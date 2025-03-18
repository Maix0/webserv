/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scanString.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:12:14 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

namespace toml {
	void Parser::Context::scanString(std::string::iterator p,
									 std::size_t		   lineno,
									 bool				   dot_is_special) {
		std::string::iterator orig = p;

		if (*p == '\'') {
			for (p++; *p && *p != '\n' && *p != '\''; p++)
				;
			if (*p != '\'') {
				std::stringstream ss;
				ss << "unterminated s-quote: line " << lineno;
				throw SyntaxError(ss.str());
			}

			this->setToken(STRING, lineno, orig, std::distance(orig, p + 1));
			return;
		}

		if (*p == '\"') {
			int	 hexreq = 0; /* #hex required */
			bool escape = false;
			for (p++; p != this->buffer.end(); p++) {
				if (escape) {
					static const std::string ESPACE_CHARS = "btnfr\"\\";
					escape								  = false;
					if (ESPACE_CHARS.find(*p) != std::string::npos)
						continue;
					if (*p == 'u') {
						hexreq = 4;
						continue;
					}
					if (*p == 'U') {
						hexreq = 8;
						continue;
					}

					{
						std::stringstream ss;
						ss << "bad escape char: line " << lineno;
						throw SyntaxError(ss.str());
					}
				}
				if (hexreq != 0) {
					hexreq--;
					static const std::string HEX_DIGIT = "0123456789ABCDEF";
					if (HEX_DIGIT.find(*p) != std::string::npos)
						continue;
					{
						std::stringstream ss;
						ss << "bad escape char: line " << lineno;
						throw SyntaxError(ss.str());
					}
				}
				if (*p == '\\') {
					escape = 1;
					continue;
				}
				if (*p == '\n')
					break;
				if (*p == '"')
					break;
			}
			if (*p != '"') {
				std::stringstream ss;
				ss << "unterminated quote: line " << lineno;
				throw SyntaxError(ss.str());
			}

			this->setToken(STRING, lineno, orig, std::distance(orig, p + 1));
			return;
		}

		/* literals */
		for (; *p && *p != '\n'; p++) {
			static const std::string DIGITS = "0123456789+-_.";

			char ch							= *p;

			if (ch == '.' && dot_is_special)
				break;
			if ('A' <= ch && ch <= 'Z')
				continue;
			if ('a' <= ch && ch <= 'z')
				continue;

			if (DIGITS.find(ch) != std::string::npos)
				continue;
			break;
		}

		this->setToken(STRING, lineno, orig, std::distance(orig, p));
		return;
	}
}  // namespace toml

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context_normalizeBasicStr.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/27 15:26:42 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include <string>
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

/*
 * Convert src to raw unescaped utf-8 string.
 * Returns NULL if error with errmsg in errbuf.
 */
std::string TomlParser::Context::normalizeBasicString(std::string::const_iterator src,
													  std::string::const_iterator end, std::size_t lineno) {
	std::string					dst;
	char						chr;

	/* scan forward on src */
	for (;;) {
		/* finished? */
		if (src == end)
			break;

		chr = *src++;
		if (chr != '\\') {
			// a plain copy suffice
			dst.push_back(chr);
			continue;
		}

		/* get the escaped char */
		chr = *src++;
		switch (chr) {
			case 'u':
			case 'U': {
				std::stringstream ss;
				ss << "unsupported sytax: unicode esacpe: line " << lineno;
				throw SyntaxError(ss.str());
				break;
			}
			case 'b':
				chr = '\b';
				break;
			case 't':
				chr = '\t';
				break;
			case 'n':
				chr = '\n';
				break;
			case 'f':
				chr = '\f';
				break;
			case 'r':
				chr = '\r';
				break;
			case '"':
				chr = '"';
				break;
			case '\\':
				chr = '\\';
				break;
			default: {
				std::stringstream ss;
				ss << "invalid escape char: line " << lineno;
				throw SyntaxError(ss.str());
			}
		}
		dst.push_back(chr);
	}
	return dst;
}

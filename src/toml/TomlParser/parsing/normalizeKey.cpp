/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context_normalizeKey.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/27 15:31:18 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

std::string TomlParser::Context::normalizeKey(Token& stok) {
	std::string::const_iterator s_start	   = this->buffer.begin() + stok.pos;
	std::string::const_iterator s_end	   = this->buffer.begin() + stok.pos + stok.raw.size();
	char						char_start = *s_start;

	/* handle quoted string */
	if (char_start == '\'' || char_start == '\"') {
		s_start++, s_end--;
		std::string ret;

		if (char_start == '\'')
			ret = std::string(s_start, s_end);
		else
			ret = this->normalizeBasicString(s_start, s_end, stok.line);

		/* newlines are not allowed in keys */
		if (ret.find('\n') != std::string::npos) {
			std::stringstream ss;
			ss << "invalid key: line " << stok.line;
			throw ForbiddenError(ss.str());
			return 0;
		}
		return ret;
	} else {
		/* for bare-key allow only this regex: [A-Za-z0-9_-]+ */
		for (std::string::const_iterator it = s_start; it != s_end; it++) {
			static const std::string BARE_KEY_CHARS =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789"
				"-_";

			if (BARE_KEY_CHARS.find(*it) != std::string::npos)
				continue;

			std::stringstream ss;
			ss << "invalid key: line " << stok.line;
			throw ForbiddenError(ss.str());
		}

		return std::string(s_start, s_end);
	}
}

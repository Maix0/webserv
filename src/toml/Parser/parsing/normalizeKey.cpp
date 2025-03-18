/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normalizeKey.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/14 10:46:40 by bgoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

using std::string;
using std::stringstream;

namespace toml {
	string Parser::Context::normalizeKey(Token& stok) {
		string::const_iterator s_start	  = this->buffer.begin() + stok.pos;
		string::const_iterator s_end	  = this->buffer.begin() + stok.pos + stok.raw.size();
		char				   char_start = *s_start;

		/* handle quoted string */
		if (char_start == '\'' || char_start == '\"') {
			s_start++, s_end--;
			string ret;

			if (char_start == '\'')
				ret = string(s_start, s_end);
			else
				ret = this->normalizeBasicString(s_start, s_end, stok.line);

			/* newlines are not allowed in keys */
			if (ret.find('\n') != string::npos) {
				stringstream ss;
				ss << "invalid key: line " << stok.line;
				throw ForbiddenError(ss.str());
				return 0;
			}
			return ret;
		} else {
			/* for bare-key allow only this regex: [A-Za-z0-9_-]+ */
			for (string::const_iterator it = s_start; it != s_end; it++) {
				static const string BARE_KEY_CHARS =
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"abcdefghijklmnopqrstuvwxyz"
					"0123456789"
					"-_";

				if (BARE_KEY_CHARS.find(*it) != string::npos)
					continue;

				stringstream ss;
				ss << "invalid key: line " << stok.line;
				throw ForbiddenError(ss.str());
			}

			return string(s_start, s_end);
		}
	}
}  // namespace toml

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nextToken.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:11:25 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

namespace toml {
	void Parser::State::nextToken(bool dotisspecial) {
		std::size_t			  lineno = this->tok.line;
		std::string::iterator p		 = (this->buffer.begin() + this->tok.pos);

		/* eat this tok */
		for (std::string::iterator it = this->tok.raw.begin(); it != this->tok.raw.end(); it++) {
			if (*it == '\n')
				lineno++;
			p++;
		}

		/* make next tok */
		while (p != this->buffer.end()) {
			/* skip comment. stop just before the \n. */
			if (*p == '#') {
				for (p++; p != this->buffer.end() && *p != '\n'; p++)
					;
				continue;
			}

			if (dotisspecial && *p == '.') {
				this->setToken(DOT, lineno, p, 1);
				return;
			}

			switch (*p) {
				case ',':
					this->setToken(COMMA, lineno, p, 1);
					return;
				case '=':
					this->setToken(EQUAL, lineno, p, 1);
					return;
				case '{':
					this->setToken(LBRACE, lineno, p, 1);
					return;
				case '}':
					this->setToken(RBRACE, lineno, p, 1);
					return;
				case '[':
					this->setToken(LBRACKET, lineno, p, 1);
					return;
				case ']':
					this->setToken(RBRACKET, lineno, p, 1);
					return;
				case '\n':
					this->setToken(NEWLINE, lineno, p, 1);
					return;
				case '\r':
				case ' ':
				case '\t':
					/* ignore white spaces */
					p++;
					continue;
			}

			this->scanString(p, lineno, dotisspecial);
			return;
		}

		this->setEof(lineno);
		return;
	}
}  // namespace toml

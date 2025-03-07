/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseTable.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:38:00 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)
#define FLINE		 __FILE__ ":" TOSTRING(__LINE__)

namespace toml {
	void Parser::Context::parseInlineTable(Value& tab, std::size_t lineno) {
		this->eatToken(LBRACE, true, FLINE);

		for (;;) {
			if (this->tok.ty == NEWLINE) {
				std::stringstream ss;
				ss << "newline not allowed in inline table: line " << lineno;
				throw SyntaxError(ss.str());
			}

			/* until } */
			if (this->tok.ty == RBRACE)
				break;

			if (this->tok.ty != STRING) {
				std::stringstream ss;
				ss << "expected a string: line " << lineno;
				throw SyntaxError(ss.str());
			}
			this->parseKeyValue(tab);

			if (this->tok.ty == NEWLINE) {
				std::stringstream ss;
				ss << "newline not allowed in inline table: line " << lineno;
				throw SyntaxError(ss.str());
			}
			/* on comma, continue to scan for next keyval */
			if (this->tok.ty == COMMA) {
				this->eatToken(COMMA, false, FLINE);
				continue;
			}
			break;
		}

		this->eatToken(RBRACE, true, FLINE);
		tab.setReadonly(true);
	}
}  // namespace toml

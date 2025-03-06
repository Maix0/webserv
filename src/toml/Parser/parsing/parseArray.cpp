/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseArray.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:36:55 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)
#define FLINE		 __FILE__ ":" TOSTRING(__LINE__)

namespace toml {
void Parser::Context::skipNewlines(bool is_dot_special) {
	while (this->tok.ty == Parser::NEWLINE) {
		this->nextToken(is_dot_special);
		if (this->tok.eof)
			break;
	}
}

void Parser::Context::parseArray(Value& val) {
	this->eatToken(LBRACKET, false, FLINE);

	for (;;) {
		this->skipNewlines(false);

		/* until ] */
		if (this->tok.ty == RBRACKET)
			break;

		switch (this->tok.ty) {
			case STRING: {
				val.getList().push_back(this->parseString(this->tok.raw, this->tok.line));

				this->eatToken(STRING, false, FLINE);
				break;
			}

			case LBRACKET: { /* [ [array], [array] ... ] */
				/* set the array kind if this will be the first entry */
				val.getList().push_back(Value::newList());
				this->parseArray(val.getList().back());
				break;
			}

			case LBRACE: { /* [ {table}, {table} ... ] */
				/* set the array kind if this will be the first entry */
				val.getList().push_back(Value::newTable());
				this->parseInlineTable(val.getList().back(), this->tok.line);
				break;
			}

			default: {
				std::stringstream ss;
				ss << "syntax error: line " << this->tok.line;
				throw SyntaxError(ss.str());
			}
		}

		this->skipNewlines(false);

		/* on comma, continue to scan for next element */
		if (this->tok.ty == COMMA) {
			this->eatToken(COMMA, false, FLINE);
			continue;
		}
		break;
	}

	eatToken(RBRACKET, true, FLINE);
}
}  // namespace toml

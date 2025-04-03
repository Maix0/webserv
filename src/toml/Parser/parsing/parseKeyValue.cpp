/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseKeyValue.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:37:21 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cassert>
#include <exception>
#include <sstream>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)
#define FLINE		 __FILE__ ":" TOSTRING(__LINE__)

namespace toml {
	void Parser::State::parseKeyValue(Value& tab) {
		if (tab.isReadonly()) {
			std::stringstream ss;
			ss << "cannot insert new entry into existing table: line " << this->tok.line;
			throw ForbiddenError(ss.str());
		}

		Token key = this->tok;
		this->eatToken(STRING, true, FLINE);

		if (this->tok.ty == DOT) {
			/* handle inline dotted key.
			   e.g.
			   physical.color = "orange"
			   physical.shape = "round"
			*/
			Value* subtab = NULL;
			{
				std::string subtabstr = this->normalizeKey(key);
				try {
					subtab = &tab.getTable().at(subtabstr);
				} catch (const std::out_of_range&) {
				}
			}

			if (!subtab)
				subtab = &this->createTomlValueInTable(tab, key, Value::newTable);

			this->nextToken(true);
			this->parseKeyValue(*subtab);
			return;
		}

		if (this->tok.ty != EQUAL) {
			std::stringstream ss;
			ss << "missing =: line " << this->tok.line;
			throw SyntaxError(ss.str());
		}

		this->nextToken(false);

		switch (this->tok.ty) {
			case STRING: { /* key = "value" */
				Value& keyval = this->createTomlValueInTable(tab, key, Value::newNull);
				Token  val	  = this->tok;

				keyval		  = this->parseString(this->tok.raw, this->tok.line);
				this->nextToken(true);
				return;
			}

			case LBRACKET: { /* key = [ array ] */
				Value& arr = this->createTomlValueInTable(tab, key, Value::newList);
				this->parseArray(arr);
				return;
			}

			case LBRACE: { /* key = { table } */
				Value& nxttab = this->createTomlValueInTable(tab, key, Value::newTable);
				this->parseInlineTable(nxttab, this->tok.line);
				break;
			}

			default:
				std::stringstream ss;
				ss << "syntax error: line " << this->tok.line;
				throw SyntaxError(ss.str());
		}
		return;
	}
}  // namespace toml

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context_parseKeyValue.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/27 15:30:16 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cassert>
#include <exception>
#include <sstream>
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)
#define FLINE		 __FILE__ ":" TOSTRING(__LINE__)

void TomlParser::Context::parseKeyValue(TomlValue& tab) {
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
		TomlValue* subtab = NULL;
		{
			std::string subtabstr = this->normalizeKey(key);
			try {
				subtab = &tab.getTable().at(subtabstr);
			} catch (const std::out_of_range&) {
			}
		}

		if (!subtab)
			subtab = &this->createTomlValueInTable(tab, key, TomlValue::newTable);

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
			TomlValue& keyval = this->createTomlValueInTable(tab, key, TomlValue::newNull);
			Token	   val	  = this->tok;

			keyval			  = this->parseString(this->tok.raw, this->tok.line);
			this->nextToken(true);
			return;
		}

		case LBRACKET: { /* key = [ array ] */
			TomlValue& arr = this->createTomlValueInTable(tab, key, TomlValue::newList);
			this->parseArray(arr);
			return;
		}

		case LBRACE: { /* key = { table } */
			TomlValue& nxttab = this->createTomlValueInTable(tab, key, TomlValue::newTable);
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

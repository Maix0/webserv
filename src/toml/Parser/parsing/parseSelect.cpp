/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseSelect.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/14 10:46:31 by bgoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)
#define FLINE		 __FILE__ ":" TOSTRING(__LINE__)

using std::string;
using std::stringstream;

namespace toml {
	void Parser::State::parseSelect(void) {
		assert(this->tok.ty == LBRACKET);

		// true if [[
		string::const_iterator tok_ptr = this->buffer.begin() + this->tok.pos;
		// need to detect '[[' on our own because next_token() will skip whitespace,
		//  and '[ [' would be taken as '[[', which is wrong.
		bool				   llb = (tok_ptr + 1 != this->buffer.end() && *(tok_ptr + 1) == '[');

		// eat [ or [[
		this->eatToken(LBRACKET, true, FLINE);
		if (llb) {
			assert(this->tok.ty == LBRACKET);
			this->eatToken(LBRACKET, true, FLINE);
		}

		this->fillTabPath();

		// For [x.y.z] or [[x.y.z]], remove z from tpath.
		Token z = this->tabPath.back();
		this->tabPath.pop_back();

		/* set up ctx->curtab */
		this->walkTabPath();

		if (!llb) {
			/* [x.y.z] -> create z = {} in x.y */
			this->createTomlValueInTable(*this->current_table, z, Value::newTable);
			this->current_table = &this->current_table->getTable().at(this->normalizeKey(z));
		} else {
			/* [[x.y.z]] -> create z = [] in x.y */
			/* handle inline dotted key.
			   e.g.
			   physical.color = "orange"
			   physical.shape = "round"
			*/
			Value* subarr = NULL;
			{
				string zstr = this->normalizeKey(z);
				try {
					subarr = &this->current_table->getTable().at(zstr);
				} catch (const std::out_of_range&) {
				}
			}
			if (!subarr)
				subarr = &this->createTomlValueInTable(*this->current_table, z, Value::newList);
			assert(subarr != NULL);
			subarr->getList().push_back(Value::newTable());
			this->current_table = &subarr->getList().back();
		}

		if (this->tok.ty != RBRACKET) {
			stringstream ss;
			ss << "expects ]: line " << this->tok.line;
			throw ForbiddenError(ss.str());
		}
		if (llb) {
			tok_ptr = this->buffer.begin() + this->tok.pos;
			if (!((tok_ptr + 1 != this->buffer.end() && *(tok_ptr + 1) == ']'))) {
				stringstream ss;
				ss << "expects ]]: line " << this->tok.line;
				throw ForbiddenError(ss.str());
			}
			this->eatToken(RBRACKET, true, FLINE);
		}

		this->eatToken(RBRACKET, true, FLINE);

		if (this->tok.ty != NEWLINE) {
			stringstream ss;
			ss << "extra chars after ] or ]]: line " << this->tok.line;
			throw ForbiddenError(ss.str());
		}
	}
}  // namespace toml

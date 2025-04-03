/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tablePath.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/14 10:46:16 by bgoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)
#define FLINE		 __FILE__ ":" TOSTRING(__LINE__)

using std::string;
using std::stringstream;
using std::vector;

namespace toml {
	void Parser::State::walkTabPath(void) {
		Value* curtab = &this->root;

		for (vector<Token>::iterator it = this->tabPath.begin(); it != this->tabPath.end(); it++) {
			Value* next = NULL;
			string key	= this->normalizeKey(*it);
			try {
				next = &curtab->getTable().at(key);
			} catch (const std::out_of_range& e) {
				(void)(e);
			};

			switch (next ? next->getType() : -1) {
				case Value::TABLE: {
					curtab = next;
					break;
				}
				case Value::LIST: {
					if (next->getList().empty()) {
						stringstream ss;
						ss << "empty list on global key ??: line " << FLINE;
						throw InternalError(ss.str());
					}
					if (!next->getList().back().isTable()) {
						stringstream ss;
						ss << "last elem isn't a table: line " << FLINE;
						throw SyntaxError(ss.str());
					}
					curtab = &next->getList().back();
					break;
				};
				case -1: {
					// value not yet created
					curtab = &(curtab->getTable()[key] = Value::newTable());

					break;
				}
				default:
					stringstream ss;
					ss << "key already exists: line " << FLINE;
					throw SyntaxError(ss.str());
			}
		}
		/* save it */
		this->current_table = curtab;
	}

	void Parser::State::fillTabPath(void) {
		int lineno = this->tok.line;

		this->tabPath.clear();
		for (;;) {
			if (this->tok.ty != STRING) {
				stringstream ss;
				ss << "invalid or missing key: line " << lineno;
				throw SyntaxError(ss.str());
			}

			this->tabPath.push_back(this->tok);

			this->nextToken(true);

			if (this->tok.ty == RBRACKET)
				break;

			if (this->tok.ty != DOT) {
				stringstream ss;
				ss << "missing key: line " << lineno;
				throw SyntaxError(ss.str());
			}

			this->nextToken(true);
		}

		if (this->tabPath.size() <= 0) {
			stringstream ss;
			ss << "empty table selector: line " << lineno;
			throw SyntaxError(ss.str());
		}
	}
}  // namespace toml

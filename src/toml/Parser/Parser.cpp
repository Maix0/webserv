/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 09:46:16 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:36:12 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/Parser.hpp"
#include "toml/Value.hpp"

#include <fstream>
#include <istream>
#include <sstream>

#include <iostream>

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)
#define FLINE		 __FILE__ ":" TOSTRING(__LINE__)

namespace toml {

	Parser::Parser() {}
	Parser::~Parser() {}

	Value Parser::parseStream(std::istream& s) {
		(void)(s);
		std::istreambuf_iterator<char> eos;
		std::string					   full(std::istreambuf_iterator<char>(s), eos);
		Parser::Context				   ctx(full);

		for (Parser::Token tok = ctx.tok; !tok.eof; tok = ctx.tok) {
			switch (tok.ty) {
				case (NEWLINE): {
					ctx.nextToken(true);
					break;
				};

				case (STRING): {
					ctx.parseKeyValue(*ctx.current_table);
					if (ctx.tok.ty != NEWLINE) {
						std::stringstream ss;
						ss << "extra chars after value: line " << tok.line;
						throw SyntaxError(ss.str());
					}
					ctx.eatToken(NEWLINE, true, FLINE);
					break;
				};

				case (LBRACKET): {
					ctx.parseSelect();
					break;
				};

				default: {
					std::stringstream ss;
					ss << "syntax error: line " << tok.line;
					throw SyntaxError(ss.str());
				}
			}
		}
		return (ctx.root);
	}

	Value Parser::parseString(const std::string& s) {
		std::stringstream ss(s);
		return (Parser::parseStream(ss));
	}

	Value Parser::parseFile(const std::string& filename) {
		std::ifstream fs(filename.c_str());
		return (Parser::parseStream(fs));
	}
}  // namespace toml

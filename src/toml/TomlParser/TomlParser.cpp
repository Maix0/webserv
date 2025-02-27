/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 09:46:16 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/27 15:28:23 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

#include <fstream>
#include <istream>
#include <sstream>

#include <iostream>

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)
#define FLINE		 __FILE__ ":" TOSTRING(__LINE__)

TomlParser::TomlParser() {}
TomlParser::~TomlParser() {}

const char* _tok_str(int ty) {
	switch (ty) {
		case (0):
			return "INVALID ";
		case (1):
			return "DOT     ";
		case (2):
			return "COMMA   ";
		case (3):
			return "EQUAL   ";
		case (4):
			return "LBRACE  ";
		case (5):
			return "RBRACE  ";
		case (6):
			return "NEWLINE ";
		case (7):
			return "LBRACKET";
		case (8):
			return "RBRACKET";
		case (9):
			return "STRING  ";
		default:
			return "unknown ";
	}
}

TomlValue TomlParser::parseStream(std::istream& s) {
	(void)(s);
	std::istreambuf_iterator<char> eos;
	std::string					   full(std::istreambuf_iterator<char>(s), eos);
	TomlParser::Context			   ctx(full);

	for (TomlParser::Token tok = ctx.tok; !tok.eof; tok = ctx.tok) {
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

TomlValue TomlParser::parseString(const std::string& s) {
	std::stringstream ss(s);
	return (TomlParser::parseStream(ss));
}

TomlValue TomlParser::parseFile(const std::string& filename) {
	std::ifstream fs(filename.c_str());
	return (TomlParser::parseStream(fs));
}

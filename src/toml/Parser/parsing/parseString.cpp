/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseString.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:37:56 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

namespace toml {
	Value Parser::State::parseString(const std::string& str, std::size_t lineno) {
		if (str == "true")
			return Value(true);
		else if (str == "false")
			return Value(false);
		else if (str == "null")
			return Value();
		else if (str == "nan")
			return Value(Float(NAN));
		else if (str == "inf" || str == "+inf")
			return Value(Float(INFINITY));
		else if (str == "-inf")
			return Value(Float(INFINITY));
		else if (str.size() >= 2 && (str[0] == '"' || str[0] == '\'') &&
				 str[str.length() - 1] == str[0]) {
			return Value(this->normalizeBasicString(str.begin() + 1, str.end() - 1, lineno));
		}
		std::string cstr = this->normalizeDecimalString(str, lineno);
		char*		end	 = NULL;
		double		strtod_ret;
		long		strtol_ret;

		errno	   = 0;
		strtol_ret = std::strtol(cstr.c_str(), &end, 10);
		if (errno == 0 && (end != NULL && *end == 0)) {
			return (Value(strtol_ret));
		}
		errno	   = 0;
		strtod_ret = std::strtod(cstr.c_str(), &end);
		if (errno == 0 && (end != NULL && *end == 0)) {
			return (Value(strtod_ret));
		}

		std::stringstream ss;
		ss << "syntax error: invalid literal: line " << lineno;
		throw SyntaxError(ss.str());
	}
}  // namespace toml

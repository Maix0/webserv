/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normalizeDecimalString.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/28 14:30:46 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include <stdexcept>
#include "toml/TomlParser.hpp"

std::string TomlParser::Context::normalizeDecimalString(const std::string& str,
														  std::size_t		 lineno) {
	(void)str;
	std::size_t		  index = 0;
	std::stringstream ss;

	if (str.empty()) {
		std::stringstream ess;
		ess << "syntax error: empty identifier: line " << lineno;
		throw SyntaxError(ess.str());
	}
	if (str[index] == '+' || str[index] == '-')
		index++;
	if (index < str.size() && (str[index] == '_' || str[str.size() - 1] == '_')) {
		std::stringstream ess;
		ess << "syntax error: _ as first or last digit: line " << lineno;
		throw SyntaxError(ess.str());
	}

	for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
		if (*it == '_')
			continue;
		ss << *it;
	}

	return (ss.str());
}

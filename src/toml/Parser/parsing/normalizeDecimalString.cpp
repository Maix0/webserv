/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normalizeDecimalString.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:11:40 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include <stdexcept>
#include "toml/Parser.hpp"

namespace toml {
	std::string Parser::State::normalizeDecimalString(const std::string& str,
														std::size_t		   lineno) {
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
}  // namespace toml

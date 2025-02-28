/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context_createTomlValueInTable.cpp                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/27 15:26:18 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <string>
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

TomlValue& TomlParser::Context::createTomlValueInTable(TomlValue& tab,
													   Token&	  key,
													   TomlValue  (*new_val)(void)) {
	std::string key_normalized = this->normalizeKey(key);

	if (tab.getTable().count(key_normalized) != 0) {
		std::stringstream ss;
		ss << "duplicate key: line " << tok.line;
		throw ForbiddenError(ss.str());
	}
	tab.getTable().insert(std::make_pair(key_normalized,  (new_val)()));
	return (tab.getTable().at(key_normalized));
}

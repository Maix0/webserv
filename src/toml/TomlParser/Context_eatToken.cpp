/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context_eatToken.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/26 15:13:36 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

void TomlParser::Context::eatToken(TokType typ, bool dot_is_special, std::string flineno) {
	if (this->tok.ty != typ)
		throw InternalError(std::string("internal error: ") + flineno);
	this->nextToken(dot_is_special);
}

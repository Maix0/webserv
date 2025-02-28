/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context_setToken.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/26 18:37:12 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

void TomlParser::Context::setToken(TokType				 ty,
								   std::size_t			 lineno,
								   std::string::iterator p,
								   std::size_t			 len) {
	Token t;

	t.ty	  = ty;
	t.line	  = lineno;
	t.raw	  = std::string(p, p + len);
	t.eof	  = 0;
	t.pos	  = std::distance(this->buffer.begin(), p);
	this->tok = t;
}

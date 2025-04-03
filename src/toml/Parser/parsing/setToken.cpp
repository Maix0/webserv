/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setToken.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:12:18 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "runtime/Logger.hpp"
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

namespace toml {
	void Parser::State::setToken(TokType				 ty,
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
}  // namespace toml

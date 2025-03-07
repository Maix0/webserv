/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eatToken.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:53:53 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:11:19 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

namespace toml {
	void Parser::Context::eatToken(TokType typ, bool dot_is_special, std::string flineno) {
		if (this->tok.ty != typ)
			throw InternalError(std::string("internal error: ") + flineno);
		this->nextToken(dot_is_special);
	}
}  // namespace toml

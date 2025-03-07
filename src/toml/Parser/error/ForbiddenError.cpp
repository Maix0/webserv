/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ForbiddenError.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:37:39 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:12:46 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/Parser.hpp"

namespace toml {
	Parser::ForbiddenError::ForbiddenError(std::string m) : msg(m) {};
	Parser::ForbiddenError::~ForbiddenError() throw() {};
	const char* Parser::ForbiddenError::what() const throw() {
		return (this->msg.c_str());
	};
}  // namespace toml

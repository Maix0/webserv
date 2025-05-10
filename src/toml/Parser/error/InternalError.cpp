/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InternalError.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:37:39 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:12:51 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/Parser.hpp"

namespace toml {
	Parser::InternalError::InternalError(std::string m) : msg(m) {}
	Parser::InternalError::~InternalError() throw() {}
	const char* Parser::InternalError::what() const throw() {
		return (this->msg.c_str());
	}
}	// namespace toml

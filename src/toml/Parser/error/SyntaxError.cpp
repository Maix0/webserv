/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SyntaxError.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:37:39 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:12:56 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/Parser.hpp"

namespace toml {
	Parser::SyntaxError::SyntaxError(std::string m) : msg(m) {};
	Parser::SyntaxError::~SyntaxError() throw() {};
	const char* Parser::SyntaxError::what() const throw() {
		return (this->msg.c_str());
	}
};	// namespace toml

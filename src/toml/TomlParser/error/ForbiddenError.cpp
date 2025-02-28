/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ForbiddenError.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:37:39 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/26 16:09:13 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/TomlParser.hpp"

TomlParser::ForbiddenError::ForbiddenError(std::string m) : msg(m) {};
TomlParser::ForbiddenError::~ForbiddenError() throw() {};
const char* TomlParser::ForbiddenError::what() const throw() {
	return (this->msg.c_str());
};

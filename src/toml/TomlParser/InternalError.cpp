/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InternalError.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:37:39 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/26 15:08:47 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/TomlParser.hpp"

TomlParser::InternalError::InternalError(std::string m) : msg(m) {};
TomlParser::InternalError::~InternalError() throw() {};
const char* TomlParser::InternalError::what() const throw() {
	return (this->msg.c_str());
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SyntaxError.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:37:39 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/26 14:38:47 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/TomlParser.hpp"

TomlParser::SyntaxError::SyntaxError(std::string m) : msg(m) {};
TomlParser::SyntaxError::~SyntaxError() throw() {};
const char* TomlParser::SyntaxError::what() const throw() {
	return (this->msg.c_str());
};

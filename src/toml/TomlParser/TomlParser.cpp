/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 09:46:16 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/26 10:11:39 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

#include <fstream>
#include <istream>
#include <sstream>
#include <stdexcept>

#include <iostream>

TomlParser::TomlParser() {}
TomlParser::~TomlParser() {}

TomlValue TomlParser::parseStream(std::istream& stream) {
	(void)(stream);
	std::istreambuf_iterator<char> eos;
	std::string					   full(std::istreambuf_iterator<char>(stream), eos);
	std::cout << full << std::endl;
	throw std::runtime_error((std::string("Unimplemented: ") + __PRETTY_FUNCTION__ + " in " + __FILE__).c_str());
}

TomlValue TomlParser::parseString(const std::string& s) {
	std::stringstream ss(s);

	return (TomlParser::parseStream(ss));
}

TomlValue TomlParser::parseFile(const std::string& filename) {
	std::ifstream file(filename.c_str());

	return (TomlParser::parseStream(file));
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 09:39:19 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/26 10:39:16 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <istream>
#include "TomlValue.hpp"

class TomlParser {
private:
	TomlParser(void);
	~TomlParser(void);

public:
	static TomlValue parseString(const std::string& s);
	static TomlValue parseFile(const std::string& filename);
	static TomlValue parseStream(std::istream& stream);

private:
	enum TokType {
		INVALID,
		DOT,
		COMMA,
		EQUAL,
		LBRACE,
		RBRACE,
		NEWLINE,
		LBRACKET,
		RBRACKET,
		STRING,
	};

	// implemetation detail
	struct Token {
		int			line;
		std::string raw;
		TokType		ty;
	};

	struct TokenIterator {
		int			current_line;
		std::string data;
	};
};

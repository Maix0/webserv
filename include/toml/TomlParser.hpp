/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 09:39:19 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/28 13:34:28 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <istream>
#include "toml/TomlValue.hpp"

#define _ERROR(NAME)                              \
	class NAME : public std::exception {          \
	public:                                       \
		virtual const char* what() const throw(); \
	};

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
		std::size_t line;
		std::size_t pos;
		std::string raw;

		TokType		ty;
		bool		eof;
	};

	class Context {
	public:
		TomlValue		   root;
		TomlValue*		   current_table;
		Token			   tok;

		std::string		   buffer;

		std::vector<Token> tabPath;

		Context&		   operator=(const Context&);
		Context(const Context&);
		explicit Context(std::string);
		~Context(void);

		void nextToken(bool is_dot_special);
		void parseKeyValue(TomlValue& current_table);
		void eatToken(TokType tok, bool dot_is_special, std::string flineno);
		void parseSelect(void);
		void setToken(TokType tok, std::size_t lineno, std::string::iterator p, std::size_t len);
		void scanString(std::string::iterator p, std::size_t lineno, bool dot_is_special);
		void setEof(std::size_t lineno);

		std::string normalizeKey(Token& tok);
		std::string normalizeBasicString(std::string::const_iterator src,
										 std::string::const_iterator end,
										 std::size_t				 lineno);
		TomlValue&	createTomlValueInTable(TomlValue& tab, Token& key, TomlValue (*newVal)(void));

		void		parseArray(TomlValue& val);
		TomlValue	parseString(const std::string& str, std::size_t lineno);
		void		parseInlineTable(TomlValue& val, std::size_t lineno);
		void		skipNewlines(bool is_dot_special);
		void		fillTabPath(void);
		void		walkTabPath(void);
	};

public:
	class SyntaxError : public std ::exception {
	private:
		std::string msg;

	public:
		SyntaxError(std::string);
		virtual ~SyntaxError() throw();
		virtual const char* what() const throw();
	};

	class InternalError : public std ::exception {
	private:
		std::string msg;

	public:
		InternalError(std::string);
		virtual ~InternalError() throw();
		virtual const char* what() const throw();
	};

	class ForbiddenError : public std ::exception {
	private:
		std::string msg;

	public:
		ForbiddenError(std::string);
		virtual ~ForbiddenError() throw();
		virtual const char* what() const throw();
	};
};

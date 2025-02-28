/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:39:19 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/27 15:25:53 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include "toml/TomlParser.hpp"
#include "toml/TomlValue.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)
#define FLINE		 __FILE__ ":" TOSTRING(__LINE__)

TomlParser::Context::Context(std::string buffer)
	: root(TomlTable()), current_table(&root), tok(Token()), buffer(buffer) {
	this->tok.line = 0;
	this->tok.eof  = false;
	this->tok.ty   = NEWLINE;
};

TomlParser::Context::~Context() {};
TomlParser::Context::Context(const TomlParser::Context& rhs)
	: root(rhs.root), current_table(&root), tok(Token()), buffer(rhs.buffer) {
	this->tok.line = 0;
	this->tok.eof  = false;
	this->tok.ty   = NEWLINE;
};

TomlParser::Context& TomlParser::Context::operator=(const TomlParser::Context& rhs) {
	if (this != &rhs) {
		this->buffer		= rhs.buffer;

		this->root			= rhs.root;
		this->current_table = &this->root;

		this->tok.eof		= false;
		this->tok.line		= 0;
		this->tok.ty		= NEWLINE;
	}
	return (*this);
}

void TomlParser::Context::setEof(std::size_t lineno) {
	this->setToken(NEWLINE, lineno, this->buffer.end(), 0);
	this->tok.eof = 1;
}

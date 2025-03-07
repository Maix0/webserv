/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 14:39:19 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:36:18 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include "toml/Parser.hpp"
#include "toml/Value.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x)	 STRINGIFY(x)
#define FLINE		 __FILE__ ":" TOSTRING(__LINE__)

namespace toml {
	Parser::Context::Context(std::string buffer)
		: root(Table()), current_table(&root), tok(Token()), buffer(buffer) {
		this->tok.line = 0;
		this->tok.eof  = false;
		this->tok.ty   = NEWLINE;
	};

	Parser::Context::~Context() {};
	Parser::Context::Context(const Parser::Context& rhs)
		: root(rhs.root), current_table(&root), tok(Token()), buffer(rhs.buffer) {
		this->tok.line = 0;
		this->tok.eof  = false;
		this->tok.ty   = NEWLINE;
	};

	Parser::Context& Parser::Context::operator=(const Parser::Context& rhs) {
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

	void Parser::Context::setEof(std::size_t lineno) {
		this->setToken(NEWLINE, lineno, this->buffer.end(), 0);
		this->tok.eof = 1;
	}
}  // namespace toml

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 13:25:52 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/26 10:16:13 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iomanip>
#include <iostream>
#include <sstream>

#include "toml/TomlValue.hpp"

#define STRING_ESCAPE_CASE(CHAR, ESCAPED) \
	case CHAR:                            \
		buf << ESCAPED;                   \
		break;

static bool _print_is_utf8(char chr) {
	// is the hight  bit set (aka not ascii !)
	return (chr & 0x80);
}

void _print_handle_utf8(std::stringstream& buf, TomlString::const_iterator& it, TomlString::const_iterator end) {
	if (it == end)
		return;	 // Handle empty case

	unsigned char first_byte = static_cast<unsigned char>(*it++);
	unsigned int  codepoint	 = 0;
	int			  num_bytes	 = 0;

	if ((first_byte & 0x80) == 0) {
		return;
	} else if ((first_byte & 0xE0) == 0xC0) {
		// 2-byte sequence
		codepoint = first_byte & 0x1F;
		num_bytes = 1;
	} else if ((first_byte & 0xF0) == 0xE0) {
		// 3-byte sequence
		codepoint = first_byte & 0x0F;
		num_bytes = 2;
	} else if ((first_byte & 0xF8) == 0xF0) {
		// 4-byte sequence
		codepoint = first_byte & 0x07;
		num_bytes = 3;
	} else {
		// Invalid UTF-8 start byte
		codepoint = 0xFFFD;	 // Unicode replacement character
	}

	for (int i = 0; i < num_bytes; ++i) {
		if (it == end) {
			codepoint = 0xFFFD;
			break;
		}
		unsigned char next_byte = static_cast<unsigned char>(*it++);
		if ((next_byte & 0xC0) != 0x80) {
			codepoint = 0xFFFD;	 // Unexpected end of string
			break;
		}
		codepoint = (codepoint << 6) | (next_byte & 0x3F);
	}

	if (codepoint <= 0xFFFF)
		buf << "\\u" << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << codepoint;
	else
		buf << "\\U" << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << codepoint;

	buf << std::dec;
	return;
}
static std::ostream& _print_string_toml(std::ostream& lhs, const TomlString& rhs) {
	std::stringstream buf;
	buf << '"';
	for (TomlString::const_iterator it = rhs.begin(); it != rhs.end(); it++) {
		switch (*it) {
			STRING_ESCAPE_CASE('\b', "\\b");
			STRING_ESCAPE_CASE('\t', "\\t");
			STRING_ESCAPE_CASE('\n', "\\n");
			STRING_ESCAPE_CASE('\f', "\\f");
			STRING_ESCAPE_CASE('\r', "\\r");
			STRING_ESCAPE_CASE('"', "\\\"");
			STRING_ESCAPE_CASE('\\', "\\\\");

			default:
				if (_print_is_utf8(*it)) {
					_print_handle_utf8(buf, it, rhs.end());
					if (it == rhs.end())
						it--;
				} else
					buf << *it;
				break;
		}
	}
	buf << '"';
	lhs << buf.str();

	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const TomlValue& rhs) {
	switch (rhs.getType()) {
		case TomlValue::BOOL: {
			lhs << (rhs.getBool() ? "true" : "false");
			break;
		};
		case TomlValue::LIST: {
			const TomlList& l = rhs.getList();

			lhs << "[";
			TomlList::const_iterator it = l.begin();
			if (it != l.end())
				lhs << *(it++);
			for (; it != l.end(); it++)
				lhs << ", " << *it;
			lhs << "]";
			break;
		};
		case TomlValue::TABLE: {
			const TomlTable& l = rhs.getTable();

			lhs << "{";
			TomlTable::const_iterator it = l.begin();
			if (it != l.end()) {
				_print_string_toml(lhs, it->first);
				lhs << " = " << it->second;
				it++;
			}
			for (; it != l.end(); it++) {
				lhs << ", ";
				_print_string_toml(lhs, it->first);
				lhs << " = " << it->second;
			}
			lhs << "}";
			break;
		};
		case TomlValue::INT: {
			lhs << rhs.getInt();
			break;
		};
		case TomlValue::FLOAT: {
			lhs << rhs.getFloat();
			break;
		};
		case TomlValue::NULL_: {
			lhs << rhs.getNull();
			break;
		};
		case TomlValue::STRING: {
			_print_string_toml(lhs, rhs.getString());
			break;
		};
	}
	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const TomlNull& rhs) {
	(void)rhs;
	return lhs << "null";
}

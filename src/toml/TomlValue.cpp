/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlValue.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 23:56:11 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/23 23:38:35 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/TomlValue.hpp"
#include <iostream>

#define IMPL_GETTERS(NAME, FIELD, TY, ETYPE)                 \
	const TY& TomlValue::get##NAME() const {                 \
		if (!this->is##NAME())                               \
			throw TomlValue::InvalidType();                  \
		return (this->raw._##FIELD.raw);                     \
	};                                                       \
	TY& TomlValue::get##NAME() {                             \
		if (!this->is##NAME())                               \
			throw TomlValue::InvalidType();                  \
		return (this->raw._##FIELD.raw);                     \
	}                                                        \
	bool TomlValue::is##NAME() const {                       \
		return (this->raw._##FIELD.tag == TomlValue::ETYPE); \
	}                                                        \
	TomlValue::RawTomlValue::RawTomlValue(TY rhs) {          \
		this->_##FIELD.tag = TomlValue::ETYPE;               \
		new (&this->_##FIELD.raw) TY(rhs);                   \
	}                                                        \
	TomlValue TomlValue::new##NAME() {                       \
		return (TomlValue(TY()));                            \
	}

#define IMPL_GETTERS_PTR(NAME, FIELD, TY, ETYPE)             \
	const TY& TomlValue::get##NAME() const {                 \
		if (!this->is##NAME())                               \
			throw TomlValue::InvalidType();                  \
		return (*this->raw._##FIELD.raw);                    \
	};                                                       \
	TY& TomlValue::get##NAME() {                             \
		if (!this->is##NAME())                               \
			throw TomlValue::InvalidType();                  \
		return (*this->raw._##FIELD.raw);                    \
	}                                                        \
	bool TomlValue::is##NAME() const {                       \
		return (this->raw._##FIELD.tag == TomlValue::ETYPE); \
	}                                                        \
	TomlValue::RawTomlValue::RawTomlValue(TY rhs) {          \
		this->_##FIELD.tag = TomlValue::ETYPE;               \
		this->_##FIELD.raw = new TY(rhs);                    \
	}                                                        \
	TomlValue TomlValue::new##NAME() {                       \
		return (TomlValue(TY()));                            \
	}

#define TOML_CONSTRUCTOR(TAG, TYPE, FIELD) \
	TomlValue::TomlValue(TYPE val) {       \
		this->raw._##FIELD.tag = TAG;      \
		this->raw._##FIELD.raw = val;      \
	}
#define TOML_CONSTRUCTOR_PTR(TAG, TYPE, FIELD)  \
	TomlValue::TomlValue(TYPE val) {            \
		this->raw._##FIELD.tag = TAG;           \
		this->raw._##FIELD.raw = new TYPE(val); \
	}

#define DESTROY_CASE(TAG, TYPE, FIELD)  \
	case (TAG): {                       \
		this->raw._##FIELD.raw.~TYPE(); \
		break;                          \
	}

#define DESTROY_CASE_PTR(TAG, TYPE, FIELD) \
	case (TAG): {                          \
		delete this->raw._##FIELD.raw;     \
		break;                             \
	}

#define COPY_CASE(TAG, TYPE, FIELD)                               \
	case (TAG): {                                                 \
		this->raw._##FIELD.tag = TAG;                             \
		new (&this->raw._##FIELD.raw) TYPE(rhs.raw._##FIELD.raw); \
		break;                                                    \
	}

#define COPY_CASE_PTR(TAG, TYPE, FIELD)                           \
	case (TAG): {                                                 \
		this->raw._##FIELD.tag = TAG;                             \
		this->raw._##FIELD.raw = new TYPE(*rhs.raw._##FIELD.raw); \
		break;                                                    \
	}

const char* TomlValue::InvalidType::what() const throw() {
	return ("Invalid type requested");
}

TomlValue::~TomlValue() {
	switch (this->raw._null.tag) {
		DESTROY_CASE(NULL_, TomlNull, null);
		DESTROY_CASE(INT, TomlNumber, int);
		DESTROY_CASE(BOOL, TomlBool, bool);
		DESTROY_CASE(FLOAT, TomlFloat, float);
		DESTROY_CASE_PTR(STRING, TomlString, string);
		DESTROY_CASE_PTR(TABLE, TomlTable, table);
		DESTROY_CASE_PTR(LIST, TomlList, list);
	}
};

TomlValue::RawTomlValue::RawTomlValue() {
	this->_null.tag = NULL_;
	this->_null.raw = TomlNull();
}

TOML_CONSTRUCTOR(NULL_, TomlNull, null);
TOML_CONSTRUCTOR(INT, TomlNumber, int);
TOML_CONSTRUCTOR(BOOL, TomlBool, bool);
TOML_CONSTRUCTOR(FLOAT, TomlFloat, float);
TOML_CONSTRUCTOR_PTR(STRING, TomlString, string);
TOML_CONSTRUCTOR_PTR(TABLE, TomlTable, table);
TOML_CONSTRUCTOR_PTR(LIST, TomlList, list);

TOML_CONSTRUCTOR(FLOAT, float, float);
TOML_CONSTRUCTOR(INT, char, int);
TOML_CONSTRUCTOR(INT, int, int);
TOML_CONSTRUCTOR(INT, short, int);
TOML_CONSTRUCTOR(INT, unsigned char, int);
TOML_CONSTRUCTOR(INT, unsigned int, int);
TOML_CONSTRUCTOR(INT, unsigned long, int);
TOML_CONSTRUCTOR(INT, unsigned short, int);

TomlValue::TomlValue() {}
TomlValue::TomlValue(const char* val) {
	this->raw._string.tag = STRING;
	this->raw._string.raw = new TomlString(val);
};

TomlValue::TomlValue(char* val) {
	this->raw._string.tag = STRING;
	this->raw._string.raw = new TomlString(val);
};

TomlValue::TomlValue(const TomlValue& rhs) {
	switch (rhs.raw._null.tag) {
		COPY_CASE(NULL_, TomlNull, null);
		COPY_CASE(INT, TomlNumber, int);
		COPY_CASE(BOOL, TomlBool, bool);
		COPY_CASE(FLOAT, TomlFloat, float);
		COPY_CASE_PTR(STRING, TomlString, string);
		COPY_CASE_PTR(TABLE, TomlTable, table);
		COPY_CASE_PTR(LIST, TomlList, list);
	}
};
TomlValue& TomlValue::operator=(const TomlValue& rhs) {
	if (this != &rhs) {
		this->~TomlValue();
		switch (rhs.raw._null.tag) {
			COPY_CASE(NULL_, TomlNull, null);
			COPY_CASE(INT, TomlNumber, int);
			COPY_CASE(BOOL, TomlBool, bool);
			COPY_CASE(FLOAT, TomlFloat, float);
			COPY_CASE_PTR(STRING, TomlString, string);
			COPY_CASE_PTR(TABLE, TomlTable, table);
			COPY_CASE_PTR(LIST, TomlList, list);
		}
	}
	return (*this);
}

TomlValue::TomlType TomlValue::getType() const {
	return (this->raw._null.tag);
}

IMPL_GETTERS(Bool, bool, TomlValue::TomlBool, BOOL);
IMPL_GETTERS(Float, float, TomlValue::TomlFloat, FLOAT);
IMPL_GETTERS(Int, int, TomlValue::TomlNumber, INT);
IMPL_GETTERS(Null, null, TomlValue::TomlNull, NULL_);

IMPL_GETTERS_PTR(List, list, TomlValue::TomlList, LIST);
IMPL_GETTERS_PTR(String, string, TomlValue::TomlString, STRING);
IMPL_GETTERS_PTR(Table, table, TomlValue::TomlTable, TABLE);

std::ostream& operator<<(std::ostream& lhs, const TomlValue& rhs) {
	switch (rhs.getType()) {
		case TomlValue::BOOL: {
			lhs << (rhs.getBool() ? "true" : "false");
			break;
		};
		case TomlValue::LIST: {
			const TomlValue::TomlList& l = rhs.getList();

			lhs << "[";
			TomlValue::TomlList::const_iterator it = l.begin();
			if (it != l.end())
				lhs << *(it++);
			for (; it != l.end(); it++)
				lhs << ", " << *it;
			lhs << "]";
			break;
		};
		case TomlValue::TABLE: {
			const TomlValue::TomlTable& l = rhs.getTable();

			lhs << "{";
			TomlValue::TomlTable::const_iterator it = l.begin();
			if (it != l.end()) {
				lhs << "\"" << it->first << "\"" << " = " << it->second;
				it++;
			}
			for (; it != l.end(); it++) {
				lhs << ", ";
				lhs << "\"" << it->first << "\"" << " = " << it->second;
			}
			lhs << "}";
			break;
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
			lhs << "\"" << rhs.getString() << "\"";
			break;
		};
	}
	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const TomlValue::TomlNull& rhs) {
	(void)rhs;
	return lhs << "null";
}

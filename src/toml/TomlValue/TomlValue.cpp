/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlValue.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 23:56:11 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/27 15:28:04 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/TomlValue.hpp"

// Helper macros to evaluate the STORAGE argument
#define IF_ptr(ptr_code, inplace_code)			 ptr_code
#define IF_inplace(ptr_code, inplace_code)		 inplace_code

// Expands to IF_inplace or IF_ptr based on SWITCH
#define IF_ELSE(STORAGE, ptr_code, inplace_code) IF_##STORAGE(ptr_code, inplace_code)

const char* TomlValue::InvalidType::what(void) const throw() {
	return ("Invalid type requested");
}

//
// DESTRUCTOR
//
#define DESTROY_CASE(STORAGE, TAG, TYPE, FIELD)                                          \
	case (TAG): {                                                                        \
		IF_ELSE(STORAGE, delete this->raw._##FIELD.raw, this->raw._##FIELD.raw.~TYPE()); \
		this->raw._##FIELD.raw = IF_ELSE(STORAGE, NULL, TYPE());                         \
		break;                                                                           \
	}

TomlValue::~TomlValue() {
	switch (this->raw._null.tag) {
		DESTROY_CASE(inplace, NULL_, TomlNull, null);
		DESTROY_CASE(inplace, INT, TomlNumber, int);
		DESTROY_CASE(inplace, BOOL, TomlBool, bool);
		DESTROY_CASE(inplace, FLOAT, TomlFloat, float);
		DESTROY_CASE(ptr, STRING, TomlString, string);
		DESTROY_CASE(ptr, TABLE, TomlTable, table);
		DESTROY_CASE(ptr, LIST, TomlList, list);
	}
};

#undef DESTROY_CASE

TomlValue::TomlType TomlValue::getType(void) const {
	return (this->raw._null.tag);
}

TomlValue::RawTomlValue::RawTomlValue(void) {
	this->_null.tag = NULL_;
	this->_null.raw = TomlNull();
}

//
// CONSTRUCTORS
//

#define TOML_CONSTRUCTOR_ALIAS(STORAGE, TAG, TYPE, ALIAS, FIELD)    \
	TomlValue::TomlValue(ALIAS val) {                               \
		this->readonly		   = false;                             \
		this->raw._##FIELD.tag = TAG;                               \
		this->raw._##FIELD.raw = IF_ELSE(STORAGE, new, ) TYPE(val); \
	}

// TYPE == ALIAS
#define TOML_CONSTRUCTOR(STORAGE, TAG, TYPE, FIELD) \
	TOML_CONSTRUCTOR_ALIAS(STORAGE, TAG, TYPE, TYPE, FIELD)

TomlValue::TomlValue(void) {}
TOML_CONSTRUCTOR(inplace, NULL_, TomlNull, null);
TOML_CONSTRUCTOR(inplace, INT, TomlNumber, int);
TOML_CONSTRUCTOR(inplace, BOOL, TomlBool, bool);
TOML_CONSTRUCTOR(inplace, FLOAT, TomlFloat, float);
TOML_CONSTRUCTOR(ptr, STRING, TomlString, string);
TOML_CONSTRUCTOR(ptr, TABLE, TomlTable, table);
TOML_CONSTRUCTOR(ptr, LIST, TomlList, list);

TOML_CONSTRUCTOR_ALIAS(inplace, FLOAT, TomlFloat, float, float);
TOML_CONSTRUCTOR_ALIAS(inplace, INT, TomlNumber, char, int);
TOML_CONSTRUCTOR_ALIAS(inplace, INT, TomlNumber, int, int);
TOML_CONSTRUCTOR_ALIAS(inplace, INT, TomlNumber, short, int);
TOML_CONSTRUCTOR_ALIAS(inplace, INT, TomlNumber, unsigned char, int);
TOML_CONSTRUCTOR_ALIAS(inplace, INT, TomlNumber, unsigned int, int);
TOML_CONSTRUCTOR_ALIAS(inplace, INT, TomlNumber, unsigned long, int);
TOML_CONSTRUCTOR_ALIAS(inplace, INT, TomlNumber, unsigned short, int);

TOML_CONSTRUCTOR_ALIAS(ptr, STRING, TomlString, char*, string);
TOML_CONSTRUCTOR_ALIAS(ptr, STRING, TomlString, const char*, string);

#undef TOML_CONSTRUCTOR_ALIAS
#undef TOML_CONSTRUCTOR

//
//   COPY CONSTRUCTOR + OPERATOR=
//
#define COPY_CASE(STORAGE, TAG, TYPE, FIELD)                                       \
	case (TAG): {                                                                  \
		this->raw._##FIELD.tag = TAG;                                              \
		IF_ELSE(STORAGE, this->raw._##FIELD.raw = new TYPE(*rhs.raw._##FIELD.raw), \
				new (&this->raw._##FIELD.raw) TYPE(rhs.raw._##FIELD.raw));         \
		break;                                                                     \
	}

TomlValue::TomlValue(const TomlValue& rhs) {
	this->readonly = rhs.readonly;
	switch (rhs.raw._null.tag) {
		COPY_CASE(inplace, NULL_, TomlNull, null);
		COPY_CASE(inplace, INT, TomlNumber, int);
		COPY_CASE(inplace, BOOL, TomlBool, bool);
		COPY_CASE(inplace, FLOAT, TomlFloat, float);
		COPY_CASE(ptr, STRING, TomlString, string);
		COPY_CASE(ptr, TABLE, TomlTable, table);
		COPY_CASE(ptr, LIST, TomlList, list);
	}
};
TomlValue& TomlValue::operator=(const TomlValue& rhs) {
	if (this != &rhs) {
		this->~TomlValue();
		this->readonly = rhs.readonly;
		switch (rhs.raw._null.tag) {
			COPY_CASE(inplace, NULL_, TomlNull, null);
			COPY_CASE(inplace, INT, TomlNumber, int);
			COPY_CASE(inplace, BOOL, TomlBool, bool);
			COPY_CASE(inplace, FLOAT, TomlFloat, float);
			COPY_CASE(ptr, STRING, TomlString, string);
			COPY_CASE(ptr, TABLE, TomlTable, table);
			COPY_CASE(ptr, LIST, TomlList, list);
		}
	}
	return (*this);
}
#undef COPY_CASE

//
// GETTERS FOR UNDERLYING TYPES
//

#define IMPL_GETTERS(STORAGE, NAME, FIELD, TY, ETYPE)                                          \
	const TY& TomlValue::get##NAME(void) const {                                               \
		if (!this->is##NAME())                                                                 \
			throw TomlValue::InvalidType();                                                    \
		return (IF_ELSE(STORAGE, *, ) this->raw._##FIELD.raw);                                 \
	}                                                                                          \
	TY& TomlValue::get##NAME(void) {                                                           \
		if (!this->is##NAME())                                                                 \
			throw TomlValue::InvalidType();                                                    \
		return (IF_ELSE(STORAGE, *, ) this->raw._##FIELD.raw);                                 \
	}                                                                                          \
	bool TomlValue::is##NAME(void) const {                                                     \
		return (this->raw._##FIELD.tag == TomlValue::ETYPE);                                   \
	}                                                                                          \
	TomlValue::RawTomlValue::RawTomlValue(TY rhs) {                                            \
		this->_##FIELD.tag = TomlValue::ETYPE;                                                 \
		IF_ELSE(STORAGE, this->_##FIELD.raw = new TY(rhs), new (&this->_##FIELD.raw) TY(rhs)); \
	}                                                                                          \
	TomlValue TomlValue::new##NAME(void) {                                                     \
		return (TomlValue(TY()));                                                              \
	}

IMPL_GETTERS(inplace, Bool, bool, TomlBool, BOOL);
IMPL_GETTERS(inplace, Float, float, TomlFloat, FLOAT);
IMPL_GETTERS(inplace, Int, int, TomlNumber, INT);
IMPL_GETTERS(inplace, Null, null, TomlNull, NULL_);

IMPL_GETTERS(ptr, List, list, TomlList, LIST);
IMPL_GETTERS(ptr, String, string, TomlString, STRING);
IMPL_GETTERS(ptr, Table, table, TomlTable, TABLE);

#undef IMPL_GETTERS

bool TomlValue::isReadonly(void) const {
	return this->readonly;
}

void TomlValue::setReadonly(bool val) {
	this->readonly = val;
}

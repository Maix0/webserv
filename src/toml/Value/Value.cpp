/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlValue.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 23:56:11 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/06 13:35:23 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/Value.hpp"

namespace toml {
// Helper macros to evaluate the STORAGE argument
#define IF_ptr(ptr_code, inplace_code)			 ptr_code
#define IF_inplace(ptr_code, inplace_code)		 inplace_code

// Expands to IF_inplace or IF_ptr based on SWITCH
#define IF_ELSE(STORAGE, ptr_code, inplace_code) IF_##STORAGE(ptr_code, inplace_code)

	static const char* _toml_type_to_str(Value::Type ty) {
		switch (ty) {
			case (Value::NULL_):
				return ("Null");
			case (Value::STRING):
				return ("String");
			case (Value::LIST):
				return ("List");
			case (Value::FLOAT):
				return ("Float");
			case (Value::BOOL):
				return ("Boolean");
			case (Value::INT):
				return ("Integer");
			case (Value::TABLE):
				return ("Table");
		}
		return ("<UNKNOWN>");
	}

	const char* Value::InvalidType::what(void) const throw() {
		return (this->msg.c_str());
	}

	Value::InvalidType::InvalidType() : msg("invalid type requested") {}
	Value::InvalidType::InvalidType(Value::Type wanted, Value::Type had) {
		this->msg = std::string("invalid type requested: wanted '") + _toml_type_to_str(wanted) +
					"' but had '" + _toml_type_to_str(had) + "'";
	}
	Value::InvalidType::InvalidType(const InvalidType& rhs) : msg(rhs.msg) {}
	Value::InvalidType::~InvalidType() throw() {}

//
// DESTRUCTOR
//
#define DESTROY_CASE(STORAGE, TAG, TYPE, FIELD)                                          \
	case (TAG): {                                                                        \
		IF_ELSE(STORAGE, delete this->raw._##FIELD.raw, this->raw._##FIELD.raw.~TYPE()); \
		this->raw._##FIELD.raw = IF_ELSE(STORAGE, NULL, TYPE());                         \
		break;                                                                           \
	}

	Value::~Value() {
		switch (this->raw._null.tag) {
			DESTROY_CASE(inplace, NULL_, Null, null);
			DESTROY_CASE(inplace, INT, Number, int);
			DESTROY_CASE(inplace, BOOL, Bool, bool);
			DESTROY_CASE(inplace, FLOAT, Float, float);
			DESTROY_CASE(ptr, STRING, String, string);
			DESTROY_CASE(ptr, TABLE, Table, table);
			DESTROY_CASE(ptr, LIST, List, list);
		}
	}

#undef DESTROY_CASE

	Value::Type Value::getType(void) const {
		return (this->raw._null.tag);
	}

	Value::RawTomlValue::RawTomlValue(void) {
		this->_null.tag = NULL_;
		this->_null.raw = Null();
	}

	//
	// CONSTRUCTORS
	//

#define TOML_CONSTRUCTOR_ALIAS(STORAGE, TAG, TYPE, ALIAS, FIELD)    \
	Value::Value(ALIAS val) {                                       \
		this->readonly		   = false;                             \
		this->raw._##FIELD.tag = TAG;                               \
		this->raw._##FIELD.raw = IF_ELSE(STORAGE, new, (TYPE)) TYPE(val); \
	}

// TYPE == ALIAS
#define TOML_CONSTRUCTOR(STORAGE, TAG, TYPE, FIELD) \
	TOML_CONSTRUCTOR_ALIAS(STORAGE, TAG, TYPE, TYPE, FIELD)

	Value::Value(void) {}
	TOML_CONSTRUCTOR(inplace, NULL_, Null, null)
	TOML_CONSTRUCTOR(inplace, INT, Number, int)
	TOML_CONSTRUCTOR(inplace, BOOL, Bool, bool)
	TOML_CONSTRUCTOR(inplace, FLOAT, Float, float)
	TOML_CONSTRUCTOR(ptr, STRING, String, string)
	TOML_CONSTRUCTOR(ptr, TABLE, Table, table)
	TOML_CONSTRUCTOR(ptr, LIST, List, list)

	TOML_CONSTRUCTOR_ALIAS(inplace, FLOAT, Float, float, float)
	TOML_CONSTRUCTOR_ALIAS(inplace, INT, Number, char, int)
	TOML_CONSTRUCTOR_ALIAS(inplace, INT, Number, int, int)
	TOML_CONSTRUCTOR_ALIAS(inplace, INT, Number, short, int)
	TOML_CONSTRUCTOR_ALIAS(inplace, INT, Number, unsigned char, int)
	TOML_CONSTRUCTOR_ALIAS(inplace, INT, Number, unsigned int, int)
	TOML_CONSTRUCTOR_ALIAS(inplace, INT, Number, unsigned long, int)
	TOML_CONSTRUCTOR_ALIAS(inplace, INT, Number, unsigned short, int)

	TOML_CONSTRUCTOR_ALIAS(ptr, STRING, String, char*, string)
	TOML_CONSTRUCTOR_ALIAS(ptr, STRING, String, const char*, string)

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

	Value::Value(const Value& rhs) {
		this->readonly = rhs.readonly;
		switch (rhs.raw._null.tag) {
			COPY_CASE(inplace, NULL_, Null, null);
			COPY_CASE(inplace, INT, Number, int);
			COPY_CASE(inplace, BOOL, Bool, bool);
			COPY_CASE(inplace, FLOAT, Float, float);
			COPY_CASE(ptr, STRING, String, string);
			COPY_CASE(ptr, TABLE, Table, table);
			COPY_CASE(ptr, LIST, List, list);
		}
	}
	Value& Value::operator=(const Value& rhs) {
		if (this != &rhs) {
			this->~Value();
			this->readonly = rhs.readonly;
			switch (rhs.raw._null.tag) {
				COPY_CASE(inplace, NULL_, Null, null);
				COPY_CASE(inplace, INT, Number, int);
				COPY_CASE(inplace, BOOL, Bool, bool);
				COPY_CASE(inplace, FLOAT, Float, float);
				COPY_CASE(ptr, STRING, String, string);
				COPY_CASE(ptr, TABLE, Table, table);
				COPY_CASE(ptr, LIST, List, list);
			}
		}
		return (*this);
	}
#undef COPY_CASE

	//
	// GETTERS FOR UNDERLYING TYPES
	//

#define IMPL_GETTERS(STORAGE, NAME, FIELD, TY, ETYPE)                                          \
	const TY& Value::get##NAME(void) const {                                                   \
		if (!this->is##NAME())                                                                 \
			throw Value::InvalidType(ETYPE, this->getType());                                  \
		return (IF_ELSE(STORAGE, *, *&) this->raw._##FIELD.raw);                                 \
	}                                                                                          \
	TY& Value::get##NAME(void) {                                                               \
		if (!this->is##NAME())                                                                 \
			throw Value::InvalidType(ETYPE, this->getType());                                  \
		return (IF_ELSE(STORAGE, *, *&) this->raw._##FIELD.raw);                                 \
	}                                                                                          \
	bool Value::is##NAME(void) const {                                                         \
		return (this->raw._##FIELD.tag == Value::ETYPE);                                       \
	}                                                                                          \
	Value::RawTomlValue::RawTomlValue(TY rhs) {                                                \
		this->_##FIELD.tag = Value::ETYPE;                                                     \
		IF_ELSE(STORAGE, this->_##FIELD.raw = new TY(rhs), new (&this->_##FIELD.raw) TY(rhs)); \
	}                                                                                          \
	Value Value::new##NAME(void) {                                                             \
		return (Value(TY()));                                                                  \
	}

	IMPL_GETTERS(inplace, Bool, bool, Bool, BOOL)
	IMPL_GETTERS(inplace, Float, float, Float, FLOAT)
	IMPL_GETTERS(inplace, Int, int, Number, INT)
	IMPL_GETTERS(inplace, Null, null, Null, NULL_)

	IMPL_GETTERS(ptr, List, list, List, LIST)
	IMPL_GETTERS(ptr, String, string, String, STRING)
	IMPL_GETTERS(ptr, Table, table, Table, TABLE)

#undef IMPL_GETTERS

	bool Value::isReadonly(void) const {
		return this->readonly;
	}

	void Value::setReadonly(bool val) {
		this->readonly = val;
	}
}  // namespace toml

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlValue.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 23:56:11 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/23 00:39:45 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "toml/TomlValue.hpp"

#define IMPL_GETTERS(NAME, FIELD, TY, ETYPE)                               \
	const TY& TomlValue::get##NAME() const throw(TomlValue::InvalidType) { \
		if (!this->is##NAME())                                             \
			throw TomlValue::InvalidType();                                \
		return (this->raw.FIELD);                                          \
	};                                                                     \
	TY& TomlValue::get##NAME() throw(TomlValue::InvalidType) {             \
		if (!this->is##NAME())                                             \
			throw TomlValue::InvalidType();                                \
		return (this->raw.FIELD);                                          \
	}                                                                      \
	bool TomlValue::is##NAME() const {                                     \
		return (this->type == TomlValue::ETYPE);                           \
	}

const char* TomlValue::InvalidType::what() const throw() {
	return ("Invalid type requested");
}

TomlValue::~TomlValue() {};
TomlValue::TomlValue() {
	this->type = TomlValue::NULL_;
}

TomlValue::TomlValue(bool val) {
	this->raw.bool_ = val;
	this->type		= TomlValue::BOOL;
}

TomlValue::TomlValue(int val) {
	this->raw.int_ = val;
	this->type	   = TomlValue::INT;
}

TomlValue::TomlValue(double val) {
	this->raw.float_ = val;
	this->type		 = TomlValue::FLOAT;
}

TomlValue::TomlValue(std::string val) {
	this->raw.string_ = val;
	this->type		  = TomlValue::STRING;
}

TomlValue::TomlValue(std::vector<TomlValue> val) {
	this->raw.list = val;
	this->type	   = TomlValue::LIST;
}

TomlValue::TomlValue(std::map<std::string, TomlValue> val) {
	this->raw.table = val;
	this->type		= TomlValue::TABLE;
}

TomlValue::TomlValue(const TomlValue& rhs) : raw(rhs.raw), type(rhs.type) {};
TomlValue& TomlValue::operator=(const TomlValue& rhs) {
	if (this == &rhs)
		return (*this);
	this->raw  = rhs.raw;
	this->type = rhs.type;
	return (*this);
}

TomlValue::TomlType TomlValue::getType() const {
	return (this->type);
}

IMPL_GETTERS(Bool, bool_, bool, BOOL);
IMPL_GETTERS(Float, float_, double, FLOAT);
IMPL_GETTERS(Int, int_, int, INT);
IMPL_GETTERS(List, list, TomlValue::TomlList, LIST);
IMPL_GETTERS(Null, null, TomlValue::TomlNull, NULL_);
IMPL_GETTERS(String, string_, std::string, STRING);
IMPL_GETTERS(Table, table, TomlValue::TomlTable, TABLE);

std::ostream& operator<<(std::ostream& lhs, const TomlValue& rhs) {
	switch (rhs.getType()) {
		case TomlValue::BOOL:
			lhs << rhs.getBool();
			break;
		case TomlValue::LIST:
			lhs << rhs.getList();
			break;
		case TomlValue::TABLE:
			lhs << rhs.getTable();
			break;
		case TomlValue::FLOAT:
			lhs << rhs.getFloat();
			break;
		case TomlValue::INT:
			lhs << rhs.getInt();
			break;
		case TomlValue::NULL_:
			lhs << rhs.getNull();
			break;
		case TomlValue::STRING:
			lhs << "\"" << rhs.getString() << "\"";
			break;
	}
	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const TomlValue::TomlNull& rhs) {
	(void)rhs;
	return lhs << "null";
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlValue.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 23:44:37 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/23 23:36:54 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <map>
#include <string>
#include <vector>

#define _DECLARE_TOML_TY(TYPE, NAME) \
	const TYPE& get##NAME() const;   \
	TYPE&		get##NAME();         \
	bool		is##NAME() const;    \
	static TomlValue new##NAME();

#define _UNION_TYPE(TAG, TYPENAME, TYPE)   \
	struct union_##TYPENAME {              \
		TomlType tag;                      \
		TYPE	 raw;                      \
	} _##TYPENAME;                         \
	static TomlType getTagFor##TYPENAME(); \
	RawTomlValue(TYPE);

#define _UNION_TYPE_PTR(TAG, TYPENAME, TYPE) \
	struct union_##TYPENAME {                \
		TomlType tag;                        \
		TYPE*	 raw;                        \
	} _##TYPENAME;                           \
	static TomlType getTagFor##TYPENAME();   \
	RawTomlValue(TYPE);

class TomlValue {
public:
	typedef bool							 TomlBool;
	typedef double							 TomlFloat;
	typedef long							 TomlNumber;
	typedef std::map<std::string, TomlValue> TomlTable;
	typedef std::string						 TomlString;
	typedef std::vector<TomlValue>			 TomlList;
	struct TomlNull {};

	enum TomlType {
		NULL_,

		BOOL,
		FLOAT,
		INT,
		LIST,
		STRING,
		TABLE,
	};

	TomlValue(TomlBool);
	TomlValue(TomlFloat);
	TomlValue(TomlList);
	TomlValue(TomlNull);
	TomlValue(TomlNumber);
	TomlValue(TomlString);
	TomlValue(TomlTable);

	TomlValue(const char*);
	TomlValue(char*);

	TomlValue(float);

	TomlValue(char);
	TomlValue(int);
	TomlValue(short);
	TomlValue(unsigned char);
	TomlValue(unsigned int);
	TomlValue(unsigned long);
	TomlValue(unsigned short);

	TomlValue();
	~TomlValue();
	TomlValue(const TomlValue&);
	TomlValue& operator=(const TomlValue&);

	TomlType   getType() const;

	_DECLARE_TOML_TY(TomlList, List);
	_DECLARE_TOML_TY(TomlNull, Null);
	_DECLARE_TOML_TY(TomlTable, Table);
	_DECLARE_TOML_TY(TomlBool, Bool);
	_DECLARE_TOML_TY(TomlFloat, Float);
	_DECLARE_TOML_TY(TomlNumber, Int);
	_DECLARE_TOML_TY(TomlString, String);

	class InvalidType : public std::exception {
		virtual const char* what() const throw();
	};

private:
	union RawTomlValue {
		_UNION_TYPE(BOOL, bool, TomlBool);
		_UNION_TYPE(FLOAT, float, TomlFloat);
		_UNION_TYPE(INT, int, TomlNumber);
		_UNION_TYPE(NULL_, null, TomlNull);

		// Why pointers ?
		// because cpp is a bitch...
		_UNION_TYPE_PTR(LIST, list, TomlList);
		_UNION_TYPE_PTR(STRING, string, TomlString);
		_UNION_TYPE_PTR(TABLE, table, TomlTable);

		RawTomlValue();
	};

	RawTomlValue raw;
};

std::ostream& operator<<(std::ostream& lhs, const TomlValue& rhs);
std::ostream& operator<<(std::ostream& lhs, const TomlValue::TomlNull& rhs);

#undef _DECLARE_TOML_TY
#undef _UNION_TYPE
#undef _UNION_TYPE_PTR

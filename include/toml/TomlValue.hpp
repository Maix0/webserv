/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlValue.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 23:44:37 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/26 09:42:42 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <map>
#include <string>
#include <vector>

class TomlValue;

#define _TOML_GETTERS(TYPE, NAME)      \
	const TYPE& get##NAME(void) const; \
	TYPE&		get##NAME(void);       \
	bool		is##NAME(void) const;  \
	static TomlValue new##NAME(void);

#define _UNION_TYPE(TAG, TYPENAME, TYPE)       \
	struct union_##TYPENAME {                  \
		TomlType tag;                          \
		TYPE	 raw;                          \
	} _##TYPENAME;                             \
	static TomlType getTagFor##TYPENAME(void); \
	RawTomlValue(TYPE);

#define _UNION_TYPE_PTR(TAG, TYPENAME, TYPE)   \
	struct union_##TYPENAME {                  \
		TomlType tag;                          \
		TYPE*	 raw;                          \
	} _##TYPENAME;                             \
	static TomlType getTagFor##TYPENAME(void); \
	RawTomlValue(TYPE);

typedef bool							 TomlBool;
typedef double							 TomlFloat;
typedef long							 TomlNumber;
typedef std::map<std::string, TomlValue> TomlTable;
typedef std::string						 TomlString;
typedef std::vector<TomlValue>			 TomlList;
struct TomlNull {};

class TomlValue {
public:
	enum TomlType {
		NULL_ = 0,

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

	TomlValue(void);
	~TomlValue(void);
	TomlValue(const TomlValue&);
	TomlValue& operator=(const TomlValue&);

	TomlType   getType() const;

	_TOML_GETTERS(TomlList, List);
	_TOML_GETTERS(TomlNull, Null);
	_TOML_GETTERS(TomlTable, Table);
	_TOML_GETTERS(TomlBool, Bool);
	_TOML_GETTERS(TomlFloat, Float);
	_TOML_GETTERS(TomlNumber, Int);
	_TOML_GETTERS(TomlString, String);

	class InvalidType : public std::exception {
		virtual const char* what(void) const throw();
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
std::ostream& operator<<(std::ostream& lhs, const TomlNull& rhs);

#undef _TOML_GETTERS
#undef _UNION_TYPE
#undef _UNION_TYPE_PTR

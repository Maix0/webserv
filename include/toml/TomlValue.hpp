/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TomlValue.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 23:44:37 by maiboyer          #+#    #+#             */
/*   Updated: 2025/02/23 00:36:54 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <map>
#include <string>
#include <vector>

#define _DECLARE_TOML_TY(TYPE, NAME)                  \
	const TYPE& get##NAME() const throw(InvalidType); \
	TYPE&		get##NAME() throw(InvalidType);       \
	bool		is##NAME() const;

class TomlValue {
public:
	typedef std::vector<TomlValue>			 TomlList;
	typedef std::map<std::string, TomlValue> TomlTable;
	struct TomlNull {};

	enum TomlType {
		BOOL,
		FLOAT,
		INT,
		LIST,
		NULL_,
		STRING,
		TABLE,
	};

	TomlValue(void);
	TomlValue(bool);
	TomlValue(double);
	TomlValue(int);
	TomlValue(std::string);
	TomlValue(TomlList);
	TomlValue(TomlTable);

	~TomlValue();
	TomlValue(const TomlValue&);
	TomlValue& operator=(const TomlValue&);

	TomlType   getType() const;

	_DECLARE_TOML_TY(TomlList, List);
	_DECLARE_TOML_TY(TomlNull, Null);
	_DECLARE_TOML_TY(TomlTable, Table);
	_DECLARE_TOML_TY(bool, Bool);
	_DECLARE_TOML_TY(double, Float);
	_DECLARE_TOML_TY(int, Int);
	_DECLARE_TOML_TY(std::string, String);

	class InvalidType : public std::exception {
		virtual const char* what() const throw();
	};

private:
	struct RawTomlValue {
		bool		bool_;
		double		float_;
		int			int_;
		std::string string_;
		TomlTable	table;
		TomlList	list;
		TomlNull	null;
	};

	RawTomlValue raw;
	TomlType	 type;
};

std::ostream& operator<<(std::ostream& lhs, const TomlValue& rhs);
std::ostream& operator<<(std::ostream& lhs, const TomlValue::TomlNull& rhs);

#undef _DECLARE_TOML_TY

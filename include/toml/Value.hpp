/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Value.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 23:44:37 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/18 23:28:27 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include "app/IndexMap.hpp"

namespace toml {
	class Value;

#define _TOML_GETTERS(TYPE, NAME)      \
	const TYPE& get##NAME(void) const; \
	TYPE&		get##NAME(void);       \
	bool		is##NAME(void) const;  \
	static Value new##NAME(void);

#define _UNION_TYPE(TAG, TYPENAME, TYPE)   \
	struct union_##TYPENAME {              \
			Type tag;                      \
			TYPE raw;                      \
	} _##TYPENAME;                         \
	static Type getTagFor##TYPENAME(void); \
	RawTomlValue(TYPE);

#define _UNION_TYPE_PTR(TAG, TYPENAME, TYPE) \
	struct union_##TYPENAME {                \
			Type  tag;                       \
			TYPE* raw;                       \
	} _##TYPENAME;                           \
	static Type getTagFor##TYPENAME(void);   \
	RawTomlValue(TYPE);

	typedef bool						 Bool;
	typedef double						 Float;
	typedef long						 Number;
	typedef std::string					 String;
	typedef std::vector<Value>			 List;
	typedef IndexMap<std::string, Value> Table;
	struct Null {};

	class Value {
		public:
			enum Type {
				NULL_ = 0,

				BOOL,
				FLOAT,
				INT,
				LIST,
				STRING,
				TABLE,
			};

			Value(Bool);
			Value(Float);
			Value(List);
			Value(Null);
			Value(Number);
			Value(String);
			Value(Table);

			Value(const char*);
			Value(char*);

			Value(float);

			Value(char);
			Value(int);
			Value(short);
			Value(unsigned char);
			Value(unsigned int);
			Value(unsigned long);
			Value(unsigned short);

			Value(void);
			~Value(void);
			Value(const Value&);
			Value& operator=(const Value&);

			Type getType(void) const;
			bool isReadonly(void) const;
			void setReadonly(bool val);

			_TOML_GETTERS(List, List);
			_TOML_GETTERS(Null, Null);
			_TOML_GETTERS(Table, Table);
			_TOML_GETTERS(Bool, Bool);
			_TOML_GETTERS(Float, Float);
			_TOML_GETTERS(Number, Int);
			_TOML_GETTERS(String, String);

			class InvalidType : public std::exception {
				private:
					std::string msg;

				public:
					InvalidType(Type wanted, Type had);
					InvalidType(const InvalidType&);
					InvalidType();
					InvalidType operator=(const InvalidType&);
					virtual ~InvalidType() throw();
					virtual const char* what(void) const throw();
			};

		private:
			union RawTomlValue {
					_UNION_TYPE(BOOL, bool, Bool);
					_UNION_TYPE(FLOAT, float, Float);
					_UNION_TYPE(INT, int, Number);
					_UNION_TYPE(NULL_, null, Null);

					// Why pointers ?
					// because cpp is a bitch...
					_UNION_TYPE_PTR(LIST, list, List);
					_UNION_TYPE_PTR(STRING, string, String);
					_UNION_TYPE_PTR(TABLE, table, Table);

					RawTomlValue();
			};

			RawTomlValue raw;
			bool		 readonly;
	};

	std::ostream& operator<<(std::ostream& lhs, const Value& rhs);
	std::ostream& operator<<(std::ostream& lhs, const Null& rhs);

#undef _TOML_GETTERS
#undef _UNION_TYPE
#undef _UNION_TYPE_PTR

};	// namespace toml

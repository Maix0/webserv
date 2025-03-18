/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Table.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 17:37:57 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/18 17:50:51 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdexcept>
#include <utility>
#include "toml/Value.hpp"

namespace toml {

	bool Table::FindKey::operator()(const toml::Table::value_type& val) const {
		return (this->k == val.first);
	}

	Value& Table::at(const std::string& key) {
		iterator v = std::find_if(this->begin(), this->end(), FindKey(key));
		if (v == this->end())
			throw std::out_of_range("provied key isn't found");
		return v->second;
	}
	const Value& Table::at(const std::string& key) const {
		const_iterator v = std::find_if(this->begin(), this->end(), FindKey(key));
		if (v == this->end())
			throw std::out_of_range("provied key isn't found");
		return v->second;
	}

	Table::size_type Table::count(const std::string& key) const {
		const_iterator v = std::find_if(this->begin(), this->end(), FindKey(key));
		if (v == this->end())
			return 0;
		return 1;
	}

	Value& Table::operator[](const std::string& key) {
		try {
			return this->at(key);
		} catch (std::out_of_range) {
			this->push_back(std::make_pair(key, Value()));
			return this->back().second;
		}
	}

	std::pair<Table::iterator, bool> Table::insert(const value_type& value) {
		try {
			iterator slot = std::find_if(this->begin(), this->end(), FindKey(value.first));
			if (slot == this->end())
				throw std::out_of_range("provied key isn't found");

			slot->second = value.second;
			return std::make_pair(slot, true);
		} catch (std::out_of_range) {
			this->push_back(value);
			return std::make_pair(this->end()--, false);
		}
	}

};	// namespace toml

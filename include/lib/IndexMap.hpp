/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IndexMap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 23:23:14 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/02 11:21:40 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>
template <typename K, typename V>
class IndexMap : public std::vector<std::pair<K, V> > {
	private:
		struct FindKey {
				const K& k;
				FindKey(const K& key) : k(key) {}
				~FindKey() {};

				bool operator()(const std::pair<K, V>& val) const { return val.first == k; };
		};

		struct FindValue {
				const V& v;
				FindValue(const V& value) : v(value) {}
				~FindValue() {};

				bool operator()(const std::pair<K, V>& val) const { return val.second == v; };
		};

	public:
		typedef typename std::vector<std::pair<K, V> >::iterator	   iterator;
		typedef typename std::vector<std::pair<K, V> >::const_iterator const_iterator;
		typedef typename std::vector<std::pair<K, V> >::value_type	   value_type;

		V& at(const K& key) {
			iterator v = std::find_if(this->begin(), this->end(), FindKey(key));
			if (v == this->end())
				throw std::out_of_range("provied key isn't found");
			return v->second;
		}
		const V& at(const K& key) const {
			const_iterator v = std::find_if(this->begin(), this->end(), FindKey(key));
			if (v == this->end())
				throw std::out_of_range("provied key isn't found");
			return v->second;
		}

		iterator find_value(const V& val) {
			iterator v = std::find_if(this->begin(), this->end(), FindValue(val));
			return v;
		}
		const_iterator find_value(const V& val) const {
			const_iterator v = std::find_if(this->begin(), this->end(), FindValue(val));
			return v;
		}

		iterator find_key(const K& key) {
			iterator v = std::find_if(this->begin(), this->end(), FindKey(key));
			return v;
		}
		const_iterator find_key(const K& key) const {
			const_iterator v = std::find_if(this->begin(), this->end(), FindKey(key));
			return v;
		}

		std::size_t count(const K& key) const {
			const_iterator v = std::find_if(this->begin(), this->end(), FindKey(key));
			if (v == this->end())
				return 0;
			return 1;
		}

		V& operator[](const K& key) {
			try {
				return this->at(key);
			} catch (const std::out_of_range& e) {
				this->push_back(std::make_pair(key, V()));
				return this->back().second;
			}
		}

		std::pair<iterator, bool> insert(const value_type& value) {
			try {
				iterator slot = std::find_if(this->begin(), this->end(), FindKey(value.first));
				if (slot == this->end())
					throw std::out_of_range("provied key isn't found");

				slot->second = value.second;
				return std::make_pair(slot, true);
			} catch (const std::out_of_range& e) {
				this->push_back(value);
				return std::make_pair(this->end()--, false);
			}
		}
};

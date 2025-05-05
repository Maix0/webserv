/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Url.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 19:37:59 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/06 00:10:50 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "app/http/Url.hpp"

using std::string;
using std::stringstream;
using std::vector;

Url::Url(const string& url) : all(url) {
	stringstream ss(url);
	string		 part;

	while (std::getline(ss, part, '/')) {
		if (!part.empty())
			this->parts.push_back(part);
	}

	// handling query_strings
	if (!this->parts.empty()) {
		string last = this->parts.back();
		this->parts.pop_back();
		string::size_type qmark = last.find('?');
		if (qmark == string::npos) {
			this->parts.push_back(last);
		} else {
			this->parts.push_back(string(last.begin(), last.begin() + qmark));
			this->qs = string(last.begin() + qmark + 1, last.end());
		}
	}
}

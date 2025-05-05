/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Url.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 20:22:35 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/05 20:30:37 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <stdexcept>
#include <string>
#include <vector>

class Url {
	private:
		std::string				 all;
		std::string				 qs;
		std::vector<std::string> parts;

	public:
		struct ParseError : public std::runtime_error {
				ParseError(const std::string& s) : std::runtime_error(s) {};
		};

		~Url() {}
		Url() {};
		Url(const std::string& url);
		Url(const Url& rhs) : all(rhs.all), qs(rhs.qs), parts(rhs.parts) {}
		Url& operator=(const Url& rhs) {
			if (this != &rhs)
				this->all = rhs.all, this->qs = rhs.qs, this->parts = rhs.parts;
			return (*this);
		}

		const std::vector<std::string>& getParts() const { return this->parts; };
		const std::string&				getQs() { return this->qs; };
		const std::string&				getAll() { return this->all; };
};

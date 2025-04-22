/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StreamDeque.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 10:34:58 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/22 10:48:51 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <deque>
#include <iostream>
#include <streambuf>

class DequeStreamBuf : public std::streambuf {
	public:
		DequeStreamBuf() { setg(&this->dummy, &this->dummy, &this->dummy); }

		void setEof() { traits_type::eof(); }

		void push_char(char c) {
			this->buffer.push_back(c);
			update_get_area();
		}

		template <typename Iterator>
		void push_iterator(Iterator start, Iterator end) {
			for (Iterator it = start; it != end; it++)
				this->buffer.push_back(*it);
			update_get_area();
		}

		void push_string(const std::string& s) {
			for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
				push_char(*it);
		}

	protected:
		virtual int_type underflow() {
			if (gptr() < egptr())
				return traits_type::to_int_type(*gptr());

			this->current_char = this->buffer.front();
			this->buffer.pop_front();

			setg(&this->current_char, &this->current_char, &this->current_char + 1);

			return traits_type::to_int_type(this->current_char);
		}

	private:
		void update_get_area() {
			if (gptr() >= egptr()) {
				setg(&this->dummy, &this->dummy, &this->dummy);
			}
		}

		std::deque<char> buffer;
		char			 current_char;
		char			 dummy;
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PassthruDeque.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 17:41:16 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/25 18:15:19 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <deque>
#include "runtime/Logger.hpp"
class PassthruDeque : public std::deque<char> {
	private:
		bool eofOnEmpty;

	public:
		PassthruDeque() : eofOnEmpty(false) {
			LOG(debug, this);
			void print_trace();
			print_trace();
		}
		~PassthruDeque() {}

		void setEofOnEmpty() { this->eofOnEmpty = true; };
		bool getEofOnEmpty() { return this->eofOnEmpty; };
};

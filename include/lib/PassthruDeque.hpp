/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PassthruDeque.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 17:41:16 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/22 17:43:10 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <deque>
class PassthruDeque : public std::deque<char> {
	private:
		bool eofOnEmpty;

	public:
		PassthruDeque() : eofOnEmpty(false) {}
		~PassthruDeque() {}
		
		void setEofOnEmpty() { this->eofOnEmpty = true; };
		bool getEofOnEmpty() { return this->eofOnEmpty; };
};

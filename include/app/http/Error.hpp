/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 16:27:13 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/20 16:29:51 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
class ExitError : public std::exception {
	public:
		const int code;

		ExitError(int code) throw() : code(code) {};
		ExitError() throw() : code(127) {};
		virtual ~ExitError() throw() {};
		virtual const char* what() const throw() { return "should exit now"; }
};

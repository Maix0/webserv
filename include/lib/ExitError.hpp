/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExitError.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 10:52:12 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/22 11:03:35 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sstream>
#include <stdexcept>
struct ExitError : public std::runtime_error {
		const int code;
		ExitError(int code)
			: std::runtime_error(dynamic_cast<std::stringstream&>(
									 std::stringstream("Requested exit with code:") << code)
									 .str()),
			  code(code) {}
};

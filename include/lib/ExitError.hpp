/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExitError.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 10:52:12 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/24 22:43:19 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sstream>
#include <stdexcept>

struct ExitError {
		const int code;
		ExitError(int code) : code(code) {}
};

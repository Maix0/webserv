/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 15:39:45 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/08 16:20:27 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "app/http/StatusCode.hpp"
#include "config/Config.hpp"

class Response {
	public:
		static std::string createStatusPageFor(const config::Server* server, StatusCode code);
};

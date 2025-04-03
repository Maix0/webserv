/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Routing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:33:00 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/25 22:45:04 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "config/Config.hpp"

/// get the associated route for an url + server combo
/// @return Null in case of no matching routes
const config::Route* getRouteFor(const config::Server& server, const std::string& url);

std::vector<std::string> url_to_parts(const std::string& url);

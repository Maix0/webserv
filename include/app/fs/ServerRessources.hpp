/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRessources.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:23:58 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/07 15:58:26 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdexcept>
#include <string>
#include "config/Config.hpp"

std::string getFileAt(const std::string& path, config::Server* server, config::Route* route = NULL);

namespace fs {
	namespace error {
		struct NotFound : public std::runtime_error {
				NotFound(const std::string& file) : std::runtime_error(file) {};
				virtual ~NotFound() throw();
		};

		struct IsADirectory : public std::runtime_error {
				IsADirectory(const std::string& file) : std::runtime_error(file) {};
				virtual ~IsADirectory() throw();
		};
	};	// namespace error
}  // namespace fs

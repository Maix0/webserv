/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRessources.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:21:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/07 16:00:25 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>

#include "app/fs/Directory.hpp"
#include "app/fs/ServerRessources.hpp"
#include "config/Config.hpp"

using std::string;

std::string getFileAt(const std::string& path, config::Server* server, config::Route* route) {
	std::string real_path = "/";
	if (server == NULL) {
		real_path  = "./";
		real_path += path;
	} else {
		std::string root = server->root;
		if (route != NULL && route->root.hasValue())
			root = route->root.get();
		real_path = root + "/" + path;
	}

	struct stat s;
	if (stat(real_path.c_str(), &s) == 0) {
		if (S_ISDIR(s.st_mode)) {
			try {
				if (route != NULL && route->index.hasValue()) {
					real_path += route->index.get();
					return getFileAt(real_path, NULL, NULL);
				}
			} catch (const fs::error::NotFound& _e) {
			}
			if (route != NULL && route->listing) {
				Directory dir(real_path.c_str());
			} else {
				throw fs::error::IsADirectory(real_path);
			}
		} else if (S_ISREG(s.st_mode)) {
		} else {
		}
	}
	throw fs::error::NotFound(real_path);
}

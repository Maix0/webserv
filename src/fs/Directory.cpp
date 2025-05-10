/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directory.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 14:15:17 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/25 22:46:10 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/fs/Directory.hpp"
#include "runtime/Logger.hpp"

#include <dirent.h>
#include <sys/types.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>

Directory::Directory(const std::string& path) : path(path) {
	DIR* dir = opendir(this->path.c_str());
	if (dir == NULL) {
		int serr = errno;
		(void)(serr);  // log macro
		LOG(err, "Failed to open dir " << this->path << ": " << strerror(serr));
		throw std::runtime_error("Failed to open dir");
	}

	try {
		for (struct dirent* entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
			Entry e;
			e.name = std::string(entry->d_name);
			if (e.name == "." || e.name == "..")
				continue;
			this->entries.push_back(e);
		}
	} catch (const std::exception& e) {
		closedir(dir);
		LOG(err, "readdir failed/exception throw: " << e.what());
		throw std::runtime_error(e.what());
	}

	closedir(dir);
}

Directory::Directory() : path("/var/empty/default_constructor") {}
Directory::~Directory() {}
Directory::Directory(const Directory& rhs) : entries(rhs.entries), path(rhs.path) {}

Directory& Directory::operator=(const Directory& rhs) {
	if (this != &rhs) {
		this->path	  = rhs.path;
		this->entries = rhs.entries;
	}
	return (*this);
}

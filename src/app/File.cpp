/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 12:21:16 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/25 22:46:20 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/File.hpp"
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <vector>
#include "app/Logger.hpp"

FileRead::FileRead(int fd) : fd(fd), eof(false) {
	if (this->fd == -1)
		throw std::runtime_error("invalid fd (-1)");
	int flags;
	_ERR_RET_THROW(flags = fcntl(this->fd, F_GETFL));
	if (!(flags & O_RDONLY || flags & O_RDWR))
		throw std::runtime_error("invalid fd (not writable)");
	_ERR_RET_THROW(fcntl(this->fd, F_SETFL, flags | O_NONBLOCK));
	_ERR_RET_THROW(fcntl(this->fd, FD_CLOEXEC));
}

FileRead::FileRead(std::string path) : fd(-1), path(path), eof(false) {
	int res = open(path.c_str(), O_RDONLY | O_CLOEXEC | O_NONBLOCK);
	if (res == -1) {
		int serr = errno;
		(void)(serr);
		LOG(warn, "Failed to open file: " << strerror(serr));
		throw std::runtime_error("failed to open file");
	}
	this->fd = res;
}

FileWrite::FileWrite(int fd, std::vector<char> buf) : fd(fd), buff(buf) {
	if (this->fd == -1)
		throw std::runtime_error("invalid fd (-1)");
	int flags;
	_ERR_RET_THROW(flags = fcntl(this->fd, F_GETFL));
	if (!(flags & O_WRONLY || flags & O_RDWR))
		throw std::runtime_error("invalid fd (not writable)");
	_ERR_RET_THROW(fcntl(this->fd, F_SETFL, flags | O_NONBLOCK));
	_ERR_RET_THROW(fcntl(this->fd, FD_CLOEXEC));
}

FileWrite::FileWrite(std::string path, std::vector<char> buf) : fd(-1), path(path), buff(buf) {
	int res = open(path.c_str(), O_WRONLY | O_CLOEXEC | O_NONBLOCK);
	if (res == -1) {
		int serr = errno;
		(void)(serr);
		LOG(warn, "Failed to open file: " << strerror(serr));
		throw std::runtime_error("failed to open file");
	}
	this->fd = res;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TempFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:42:06 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/07 09:00:36 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <ios>
#include <stdexcept>

#include "lib/TempFile.hpp"
#include "runtime/Logger.hpp"

tiostream::tiostream() : fd(-1) {
	char data[] = "/tmp/webserv-XXXXXX";
	if ((this->fd = mkstemp(data)) == -1)
		throw std::runtime_error("Failed to create temp file");
	_ERR_RET_THROW(fcntl(this->fd, FD_CLOEXEC));
	this->filename = data;
	this->open(data, ios_base::in | ios_base::out | ios_base::binary);
	if (this->fail())
		throw std::runtime_error("Failed to open temp file");
}

tiostream::~tiostream() {
	::close(this->fd);
	::unlink(this->filename.c_str());
}

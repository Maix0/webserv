/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TempFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:42:06 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/15 21:46:31 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib/TempFile.hpp"
#include <cstdio>
#include <stdexcept>
#include "runtime/Logger.hpp"

tiostream::tiostream() {
	char data[] = "/tmp/webserv-XXXXXX";
	if (std::tmpnam(data) == NULL)
		throw std::runtime_error("Failed to create temp file");
	this->filename = data;
	this->open(data, ios_base::in | ios_base::out | ios_base::binary);
}

tiostream::~tiostream() {
	if (std::remove(this->filename.c_str()) != 0)
		LOG(warn, "failed to remove temp file " << this->filename);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 13:52:22 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/13 16:24:15 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/File.hpp"
#include <cstdio>
#include "app/Callback.hpp"
#include "app/Context.hpp"
#include "app/Logger.hpp"
#include "app/Shared.hpp"

#define READ_BUFFER_SIZE (1024 * 4)

namespace app {
	void FileReadCallback::call(Epoll& epoll, Shared<Callback> self) {
		if (this->inner->isEof()) {
			self->setFinished();
			return;
		}
		char	buffer[READ_BUFFER_SIZE] = {};
		ssize_t res						 = read(this->inner->asFd(), buffer, READ_BUFFER_SIZE);
		if (res == -1)
			throw std::runtime_error("Failed to read");
		this->inner->getBuf().insert(this->inner->getBuf().end(), &buffer[0], &buffer[res]);
		if (res == 0) {
			self->setFinished();
			return this->inner->setEof();
		} else
			epoll.addCallback(this->inner->asFd(), READ, self);
	};

	void FileWriteCallback::call(Epoll& epoll, Shared<Callback> self) {
		if (this->inner->getBuf().empty()) {
			self->setFinished();
			return;
		}

		std::vector<char>& buf = this->inner->getBuf();
		ssize_t			   res = write(this->inner->asFd(), &buf[0], buf.size());
		if (res == -1)
			throw std::runtime_error("Failed to write");

		buf.erase(buf.begin(), buf.begin() + res);
		if (!buf.empty())
			epoll.addCallback(this->inner->asFd(), WRITE, self);
		else
			self->setFinished();
	};
};	// namespace app

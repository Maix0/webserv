/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiInstance.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 16:17:28 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 13:29:06 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/CgiInstance.hpp"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include "app/State.hpp"
#include "runtime/Logger.hpp"

struct FdWrapper {
		int fd;
		~FdWrapper() {
			if (this->fd != -1)
				close(this->fd);
		};
		FdWrapper(int fd) : fd(fd) {}
		FdWrapper() : fd(-1) {}
};

void CgiInstance::spawn(std::vector<char> buf) {
	/// create a temporary file (such that we can write to it like a big dumb dumb without epoll
	/// hehe...)
	FdWrapper fd = open("/tmp", O_TMPFILE | O_CLOEXEC | O_RDWR | O_EXCL, 0777);
	if (fd.fd == -1) {
		int serr = errno;
		(void)serr;
		LOG(err, "Failed to open temporary file : " << strerror(serr));
		throw std::runtime_error("failed to open temp file");
	}
	std::size_t buf_size = buf.size();

	ssize_t ret			 = 0;
	while (!buf.empty()) {
		ret = write(fd.fd, &buf[0], buf.size());
		if (ret < 0) {
			int serr = errno;
			(void)(serr);
			LOG(err, "Failed to write to temp file: " << strerror(serr));
			throw std::runtime_error("write failure");
		}
		buf.erase(buf.begin(), buf.begin() + ret);
	}

	int _pip[2];
	if (pipe(_pip) == -1) {
		int serr = errno;
		(void)serr;
		LOG(err, "Failed to create pipe: " << strerror(serr));
		throw std::runtime_error("failed to create pipe");
	}
	FdWrapper pip[2];
	pip[0] = _pip[0];
	pip[1] = _pip[1];

	for (int idx = 0; idx < 2; idx++) {
		int flags;
		_ERR_RET_THROW(fcntl(pip[idx].fd, FD_CLOEXEC));
		_ERR_RET_THROW((flags = fcntl(pip[idx].fd, F_GETFL)));
		_ERR_RET_THROW((fcntl(pip[idx].fd, F_SETFL, flags | O_NONBLOCK)));
	}

	int pid = fork();
	if (pid == -1) {
		int serr = errno;
		(void)serr;
		LOG(err, "Failed to fork for cgi: " << strerror(serr));
		throw std::runtime_error("failed fork");
	}

	if (pid == 0) {
		char		sbuf[1024] = {};
		const char* argv[2];
		argv[0] = cgi.binary.c_str();
		argv[1] = NULL;

		{
			snprintf(sbuf, sizeof(sbuf), "%zu", buf_size);
			setenv("CONTENT-LENGTH", sbuf, 1);
		}
		setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
		setenv("CONTENT-LENGTH", "CGI/1.1", 1);

		dup2(fd.fd, STDIN_FILENO);
		dup2(pip[1].fd, STDOUT_FILENO);
		execve(cgi.binary.c_str(), (char* const*)argv, environ);
		throw std::runtime_error(0);
		//throw ExitError(127);
	}

	this->output = pip[0].fd;
	this->pid	 = pid;
	pip[0].fd	 = -1;
}

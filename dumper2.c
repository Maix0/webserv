/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dumper2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 13:18:52 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/03 17:03:45 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef _GNU_SOURCE
#	define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

extern char** environ;

#define ERR_CHECK(code)                                                                        \
	if ((code) < 0) {                                                                          \
		fprintf(stderr, "Failed check here: %s:%d for `%s`: %m\n", __FILE__, __LINE__, #code); \
		abort();                                                                               \
	}

static char READ_BUF[1 << 20];

#define BIN_PATH "/home/maix/school/ring-4/webserv/dl/cgi_tester"

int main(void) {
	char buffer[1024];
	ERR_CHECK(snprintf(buffer, sizeof(buffer), "./req_dumps/%d/", getpid()));

	mkdir("./req_dumps", S_IREAD | S_IWRITE | S_IEXEC);
	ERR_CHECK(mkdir(buffer, S_IREAD | S_IWRITE | S_IEXEC));

	int directory_fd;
	int env_fd;
	int stdin_fd;

	ERR_CHECK(directory_fd = open(buffer, O_PATH | O_CLOEXEC));
	{
		ERR_CHECK(env_fd = openat(directory_fd, "env", O_CREAT | O_TRUNC | O_WRONLY | O_CLOEXEC,
								  S_IWRITE | S_IREAD));
		for (char** envp = environ; *envp; envp++)
			dprintf(env_fd, "%s\n", *envp);
		close(env_fd);
	}
	{
		ERR_CHECK(stdin_fd = openat(directory_fd, "stdin", O_CREAT | O_TRUNC | O_RDWR | O_CLOEXEC,
									S_IWRITE | S_IREAD));
		ssize_t res;
		while ((res = read(0, READ_BUF, sizeof(READ_BUF))) > 0)
			(void)!write(stdin_fd, READ_BUF, res);
		lseek(stdin_fd, 0, SEEK_SET);
	}
	// we have dumped lots of stuff, so now we setup the pipe for fork
	int pip[2];
	ERR_CHECK(pipe2(pip, O_CLOEXEC));

	int pid = -1;
	ERR_CHECK(pid = fork());
	if (pid == 0) {
		ERR_CHECK(dup2(stdin_fd, STDIN_FILENO));
		ERR_CHECK(dup2(pip[1], STDOUT_FILENO));

		const char* argv[] = {BIN_PATH, NULL};
		execve((const char*)argv[0], (char**)argv, (char**)environ);
		return 127;
	}
	close(pip[1]);

	struct epoll_event ev;
	struct epoll_event e_arr[16];
	int				   efd;
	int				   signal_fd;
	int				   stdout_fd;

	ERR_CHECK(stdout_fd = openat(directory_fd, "stdout", O_CREAT | O_TRUNC | O_WRONLY | O_CLOEXEC,
								 S_IWRITE | S_IREAD));

	ERR_CHECK(efd = epoll_create1(SFD_CLOEXEC));
	{
		ev.data.fd = pip[0];
		ev.events  = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
		ERR_CHECK(epoll_ctl(efd, EPOLL_CTL_ADD, pip[0], &ev));
	}
	{
		sigset_t mask;

		// Block SIGCHLD so it isn't handled by default
		sigemptyset(&mask);
		sigaddset(&mask, SIGCHLD);
		if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
			perror("sigprocmask");
			exit(EXIT_FAILURE);
		}
		ERR_CHECK(signal_fd = signalfd(-1, &mask, 0));

		ev.data.fd = signal_fd;
		ev.events  = EPOLLIN;
		ERR_CHECK(epoll_ctl(efd, EPOLL_CTL_ADD, signal_fd, &ev));
	}
	while (signal_fd != -1 && pip[0] != -1) {
		int r;
		ERR_CHECK(r = epoll_wait(efd, e_arr, 16, -1));
		for (int i = 0; i < r; i++) {
			if (e_arr[i].data.fd == signal_fd) {
				struct signalfd_siginfo fdsi;
				if (read(signal_fd, &fdsi, sizeof(fdsi)) != sizeof(fdsi))
					abort();
				if (fdsi.ssi_signo == SIGCHLD) {
					epoll_ctl(efd, EPOLL_CTL_DEL, signal_fd, NULL);
					close(signal_fd);
					signal_fd = -1;
					waitpid(-1, NULL, 0);
				}
			}
			if (e_arr[i].data.fd == pip[0]) {
				if (e_arr[i].events & EPOLLIN) {
					ssize_t res = read(pip[0], READ_BUF, sizeof(READ_BUF));
					if (res < 0)
						abort();
					(void)!write(stdout_fd, READ_BUF, res);
					(void)!write(1, READ_BUF, res);
				}
				if (e_arr[i].events & (EPOLLRDHUP | EPOLLHUP)) {
					epoll_ctl(efd, EPOLL_CTL_DEL, pip[0], NULL);
					close(pip[0]);
					pip[0] = -1;
				}
			}
		}
	}
	close(efd);
	close(stdout_fd);
	close(stdin_fd);
	close(directory_fd);
	return 0;
}

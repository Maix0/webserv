/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dumper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 15:16:27 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/02 16:47:45 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char buffer[1 << 20];

void putstr_fd(int fd, const char* s) {
	if (s == NULL)
		return;
	size_t len = strlen(s);
	(void)!write(fd, s, len);
}

int main(void) {
	putstr_fd(STDERR_FILENO, "spawned\n");
	putstr_fd(STDOUT_FILENO, "Status: 200 OK\r\n");
	putstr_fd(STDOUT_FILENO, "\r\n");
	const char* l = getenv("CONTENT_LENGTH");
	if (l == NULL)
		l = "0";
	ssize_t res = 0;
	size_t	tot = 0;
	do {
		res = read(0, buffer, sizeof(buffer));
		if (res == -1) {
			int e = errno;
			putstr_fd(STDERR_FILENO, "failed to read: ");
			putstr_fd(STDERR_FILENO, strerror(e));
			putstr_fd(STDERR_FILENO, "\n");
			return 0;
		}
		tot += res;
		(void)!write(1, buffer, res);
	} while ((size_t)res == sizeof(buffer));
	putstr_fd(STDERR_FILENO, "done\n");
}

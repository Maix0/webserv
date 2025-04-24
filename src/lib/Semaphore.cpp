/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Semaphore.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 22:29:08 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/24 23:32:55 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib/Semaphore.hpp"

#ifdef ENABLE_TRUE_SEMAPHORE
#	include <fcntl.h>
#	include <semaphore.h>
#	include <unistd.h>
#	include <cerrno>
#	include <cstring>

#	include <iostream>
#	include <sstream>

#	include "lib/ExitError.hpp"

#	define SEM_INNER(OBJ) static_cast<sem_t*>((OBJ).inner)

Semaphore::Semaphore(const std::string& name, std::size_t count) : name(name), inner(NULL) {
	(void)count;
	(void)name;
	(void)this->inner;
	(void)this->name;
	std::stringstream ss;
	ss << this->name << "." << getpid();
	this->name = ss.str();

	this->inner = sem_open(this->name.c_str(), O_CREAT | O_EXCL, 0, 0);
	if (this->inner == SEM_FAILED) {
		int serrno = errno;
		std::cerr << "Failed to create semaphore, force exiting...: " << strerror(serrno)
				  << std::endl;
		this->inner = NULL;
		throw ExitError(2);
	}
	while (count--)
		sem_post(SEM_INNER(*this));
}

Semaphore::~Semaphore() {
	if (SEM_INNER(*this) != NULL) {
		sem_close(SEM_INNER(*this));
		sem_unlink(this->name.c_str());
	}
	this->inner = NULL;
}

Semaphore::Ticket::Ticket(Semaphore& parent) : parent(parent) {
	if (SEM_INNER(this->parent) != NULL)
		sem_wait(SEM_INNER(this->parent));
};
Semaphore::Ticket::~Ticket() {
	if (SEM_INNER(this->parent) != NULL)
		sem_post(SEM_INNER(this->parent));
}

#else

Semaphore::Semaphore(const std::string& name, std::size_t count) : name(name), inner(NULL) {
	(void)count;
	(void)name;
	(void)this->name;
	(void)this->inner;
}

Semaphore::~Semaphore() {}

Semaphore::Ticket::Ticket(Semaphore& parent) : parent(parent) {
	(void)this->parent;
	(void)parent;
}
Semaphore::Ticket::~Ticket() {}

#endif

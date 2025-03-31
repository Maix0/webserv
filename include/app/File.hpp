/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 11:54:20 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/25 22:44:16 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <string>
#include <vector>
#include "app/AsFd.hpp"
#include "app/Callback.hpp"
#include "app/Epoll.hpp"
#include "app/EpollType.hpp"
#include "app/Shared.hpp"

class FileRead : AsFd {
	private:
		int				  fd;
		std::string		  path;
		std::vector<char> buff;
		bool			  eof;

	public:
		FileRead(int fd);
		FileRead(std::string path);
		~FileRead() {
			if (this->fd != -1)
				close(this->fd);
		};

		inline int						asFd() { return this->fd; };
		inline std::vector<char>&		getBuf() { return this->buff; };
		inline const std::vector<char>& getBuf() const { return this->buff; };
		inline const std::string&		getPath() const { return this->path; };
		inline bool						isEof() const { return this->eof; };
		inline void						setEof() { this->eof = true; };
};

class FileReadCallback : public Callback {
	private:
		Shared<FileRead> inner;

	public:
		FileReadCallback(Shared<FileRead> file) : inner(file) {};
		~FileReadCallback() {};

		void	  call(Epoll& epoll, Shared<Callback> self);
		int		  getFd() { return this->inner->asFd(); };
		EpollType getTy() { return READ; };
};

class FileWrite : public AsFd {
	private:
		int				  fd;
		std::string		  path;
		std::vector<char> buff;

	public:
		FileWrite(int fd, std::vector<char> buf);
		FileWrite(std::string path, std::vector<char> buf);
		~FileWrite() {
			if (this->fd != -1)
				close(this->fd);
		};

		inline int						asFd() { return this->fd; };
		inline std::vector<char>&		getBuf() { return this->buff; };
		inline const std::vector<char>& getBuf() const { return this->buff; };
		inline const std::string&		getPath() const { return this->path; };
};

class FileWriteCallback : public Callback {
	private:
		Shared<FileWrite> inner;

	public:
		FileWriteCallback(Shared<FileWrite> file) : inner(file) {};
		~FileWriteCallback() {};

		void	  call(Epoll& epoll, Shared<Callback> self);
		int		  getFd() { return this->inner->asFd(); };
		EpollType getTy() { return WRITE; };
};

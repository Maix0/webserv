/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiInstance.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 16:02:55 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 13:30:34 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include "app/net/Connection.hpp"
#include "app/net/Socket.hpp"
#include "config/Config.hpp"

class CgiInstance {
	private:
		Rc<Connection>::Weak parent_connection;
		config::Cgi&		 cgi;
		pid_t				 pid;
		int					 output;
		config::Server&		 server;
		std::string			 url;
		std::string			 method;

	public:
		~CgiInstance() {
			if (this->output != -1)
				close(this->output);
			if (this->pid) {
				waitpid(this->pid, NULL, 0);
			}
			(void)(server);
		}
		CgiInstance(Rc<Connection>&	   parent,
					config::Cgi&	   cgi,
					config::Server&	   server,
					const std::string& url,
					const std::string& method)
			: parent_connection(Rc<Connection>::Weak::make_weak(parent)),
			  cgi(cgi),
			  pid(0),
			  output(-1),
			  server(server),
			  url(url),
			  method(method) {}

		void spawn(std::vector<char> request_buffer);
};

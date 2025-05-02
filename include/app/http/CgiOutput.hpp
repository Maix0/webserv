/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiOutput.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 15:01:23 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/02 17:01:51 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "app/http/Request.hpp"
#include "interface/AsFd.hpp"
#include "interface/Callback.hpp"
#include "lib/Rc.hpp"
#include "lib/TempFile.hpp"
#include "runtime/EpollType.hpp"

class Response;
class Connection;

class CgiOutput {
	private:
		Weak<Connection> conn;
		Weak<Response>	 res;
		Weak<Request>	 req;
		Rc<tiostream>	 raw_buf;

		Rc<tiostream> body;
		size_t		  body_size;

		std::string buffer;

		bool finished;
		bool finished_headers;

		int req_fd;
		int pid;

		std::string script_path;
		std::string path_info;
		std::string query_string;

		char* const* setup_env(char**					 envp,
							   std::vector<std::string>& buf,
							   std::vector<char const*>& out);
		void		 do_exec(std::string& bin);

	public:
		CgiOutput(Epoll&			 epoll,
				  Rc<Request>&		 req,
				  const config::Cgi* cgi,
				  std::string&		 cgi_suffix,
				  Rc<Response>&		 res,
				  Rc<Connection>&	 conn);
		~CgiOutput();

		void parseBytes();
		void setFinished();
		int	 getPid() { return this->pid; };

		int	 getPipeFd() { return this->req_fd; };
		bool isFinished() { return this->finished; };
};

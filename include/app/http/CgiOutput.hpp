/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiOutput.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 15:01:23 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/29 10:50:07 by maiboyer         ###   ########.fr       */
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
		class PipeInstance : public AsFd {
			private:
				Rc<CgiOutput> parent;
				Rc<Request>	  req;

				int pid;
				int rfd;

				std::string bin;

				char* const* setup_env(char** envp);

			public:
				virtual int asFd() { return this->rfd; };
				PipeInstance(std::string bin, Rc<Request> req, Rc<CgiOutput> parent);
				~PipeInstance();

			private:
				template <EpollType TY>
				class CB : public ::AsFd, public ::Callback {
					private:
						Rc<CgiOutput> parent;
						Rc<Request>	  req;
						int			  rfd;

					public:
						CB(const PipeInstance& p) : parent(p.parent), req(p.req), rfd(p.rfd) {};

						virtual int		  asFd() { return this->rfd; };
						virtual int		  getFd() { return this->asFd(); };
						virtual EpollType getTy() { return TY; };
						virtual void	  call(Epoll& epoll, Rc<Callback> self);
				};

			public:
				typedef CB<READ>   CRead;
				typedef CB<HANGUP> CHangup;
		};

	private:
		Rc<Connection>	 conn;
		Rc<PipeInstance> pipe;
		Rc<Response>	 res;

		Rc<tiostream> body;
		size_t		  body_size;

		std::string buffer;

		bool finished;
		bool finished_headers;

	public:
		CgiOutput(Epoll&		 epoll,
				  Rc<Request>&	 req,
				  std::string	 cgi_bin,
				  Rc<Response>&	 res,
				  Rc<Connection> conn);
		~CgiOutput();

		void parseBytes();
		void setFinished();

		int	 getPipeFd() { return this->pipe->asFd(); };
		bool isFinished() { return this->finished; };
};

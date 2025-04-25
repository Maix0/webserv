/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiPipe.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:04:41 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/25 18:18:02 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "app/http/Request.hpp"
#include "interface/AsFd.hpp"
#include "interface/Callback.hpp"
#include "lib/PassthruDeque.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/EpollType.hpp"

class Connection;

class PipeCgi : public AsFd {
	private:
		int				  pid;
		int				  rfd;
		Rc<Request>		  req;
		Rc<PassthruDeque> output;
		std::string		  bin;
		Rc<Connection>	  parent;

		char* const* setup_env(char** envp);

	public:
		virtual int asFd() { return this->rfd; };
		PipeCgi(std::string bin, Rc<Request> req, Rc<Connection>& parent);
		~PipeCgi();

		Rc<PassthruDeque> getBuf() { return this->output; };

		class CallbackRead : public AsFd, public Callback {
			private:
				Rc<Request>		  req;
				Rc<PassthruDeque> output;
				int				  rfd;
				Rc<Connection>	  parent;

			public:
				CallbackRead(const PipeCgi& cgi);

				virtual int		  asFd() { return this->rfd; };
				virtual int		  getFd() { return this->asFd(); };
				virtual EpollType getTy() { return READ; };
				virtual void	  call(Epoll& epoll, Rc<Callback> self);
		};

		class CallbackHangup : public AsFd, public Callback {
			private:
				Rc<Request>		  req;
				Rc<PassthruDeque> output;
				int				  rfd;
				Rc<Connection>	  parent;

			public:
				CallbackHangup(const PipeCgi& cgi);

				virtual int		  asFd() { return this->rfd; };
				virtual int		  getFd() { return this->asFd(); };
				virtual EpollType getTy() { return HANGUP; };
				virtual void	  call(Epoll& epoll, Rc<Callback> self);
		};
};

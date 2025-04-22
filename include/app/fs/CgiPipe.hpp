/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiPipe.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 11:04:41 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/22 11:22:23 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "interface/AsFd.hpp"
#include "interface/Callback.hpp"
#include "lib/StreamDeque.hpp"
#include "runtime/Epoll.hpp"
#include "runtime/EpollType.hpp"

class PipeCgi : public AsFd {
	private:
		int				   rfd;
		Rc<Request>		   req;
		Rc<Response>	   res;
		Rc<DequeStreamBuf> output;

	public:
		PipeCgi(Rc<Request> req, Rc<Response> res);

		class CallbackRead : public AsFd, public Callback {
			private:
				Rc<Request>		   req;
				Rc<Response>	   res;
				Rc<DequeStreamBuf> output;
				int				   rfd;

			public:
				CallbackRead(const PipeCgi& cgi)
					: req(cgi.req), res(cgi.res), rfd(cgi.rfd), output(cgi.output) {}

				virtual int		  asFd() { return this->rfd; };
				virtual int		  getFd() { return this->asFd(); };
				virtual EpollType getTy() { return READ; };
				virtual void	  call(Epoll& epoll, Rc<Callback> self);
		};

		class CallbackHangup : public AsFd, public Callback {
			private:
				Rc<Request>		   req;
				Rc<Response>	   res;
				Rc<DequeStreamBuf> output;
				int				   rfd;

			public:
				CallbackHangup(const PipeCgi& cgi)
					: req(cgi.req), res(cgi.res), rfd(cgi.rfd), output(cgi.output) {}

				virtual int		  asFd() { return this->rfd; };
				virtual int		  getFd() { return this->asFd(); };
				virtual EpollType getTy() { return HANGUP; };
				virtual void	  call(Epoll& epoll, Rc<Callback> self);
		};
};

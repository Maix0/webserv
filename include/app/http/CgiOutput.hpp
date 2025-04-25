/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiOutput.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 15:01:23 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/25 18:31:12 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "app/fs/CgiPipe.hpp"
#include "lib/PassthruDeque.hpp"
#include "lib/Rc.hpp"
#include "lib/TempFile.hpp"

class Response;

class CgiOutput {
	private:
		Rc<Response>	  res;
		Rc<PipeCgi>		  cgi;
		Rc<PassthruDeque> buf;
		Rc<tiostream>	  body;
		size_t			  body_size;
		bool			  finished;
		bool			  finished_headers;

	public:
		CgiOutput(Rc<PipeCgi> cgi, Rc<Response>& s);
		~CgiOutput();

		Rc<PassthruDeque>& getBuffer() { return this->buf; };
		void			   parseBytes();
		bool			   isFinished() { return this->finished; };
		void			   setFinished();

		int getPipeFd() { return this->cgi->asFd(); };
};

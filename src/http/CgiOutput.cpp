/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiOutput.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 15:00:28 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/29 10:50:59 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <string>

#include "app/http/CgiOutput.hpp"
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "app/net/Connection.hpp"
#include "lib/Functors.hpp"
#include "lib/Rc.hpp"
#include "lib/StringHelper.hpp"

void CgiOutput::parseBytes() {
	while (!this->buffer.empty()) {
		if (this->finished_headers) {
			char data[2048];
			while (!this->buffer.empty()) {
				size_t cpy_len = std::min(sizeof(data), this->buffer.size());
				for (size_t i = 0; i < cpy_len; i++)
					data[i] = this->buffer.at(i);
				this->buffer.erase(this->buffer.begin(), this->buffer.begin() + cpy_len);
				this->body->write(data, cpy_len);
				this->body_size += cpy_len;
			}
		} else {
			std::string lines(this->buffer.begin(), this->buffer.end());
			if (!lines.empty())
				LOG(debug, "line = '" << lines << "'");
			Rc<Response> r = this->res;
			while (true) {
				std::string::size_type crlf = lines.find(CRLF);
				if (crlf == std::string::npos)
					return;
				std::string l(lines.begin(), lines.begin() + crlf);
				this->buffer.erase(this->buffer.begin(), this->buffer.begin() + crlf + 2);
				lines.erase(lines.begin(), lines.begin() + crlf + 2);
				if (l.empty()) {
					this->finished_headers = true;
					continue;
				}
				LOG(info, "cgi_header: '" << l << "'");
				std::string::size_type pos = l.find(':');
				if (pos == std::string::npos) {
					LOG(warn, "invalid reponse from cgi: '" << l << "'");
				}
				std::string name(l.begin(), l.begin() + pos);
				std::string val(l.begin() + pos + 1, l.end());
				string_tolower(name);
				string_trim(name);
				string_trim(val);
				r->setHeader(name, val);
			}
		}
	}
}
CgiOutput::CgiOutput(Epoll&			epoll,
					 Rc<Request>&	req,
					 std::string	cgi_bin,
					 Rc<Response>&	res,
					 Rc<Connection> conn)
	: conn(conn),
	  pipe(Rc<PipeInstance>(Functor3<PipeInstance, std::string, Rc<Request>, Rc<CgiOutput> >(
								cgi_bin,
								req,
								Rc<CgiOutput>::fromRaw(this)),
							RCFUNCTOR)),
	  res(res) {
	(void)(cgi_bin);
	(void)(req);
	Rc<CgiOutput> self	   = Rc<CgiOutput>::fromRaw(this);
	this->body_size		   = 0;
	this->finished		   = false;
	this->finished_headers = false;
	Rc<PipeInstance::CRead>	  cbr(Functor1<PipeInstance::CRead, PipeInstance&>(*this->pipe),
								  RCFUNCTOR);
	Rc<PipeInstance::CHangup> cbh(Functor1<PipeInstance::CHangup, PipeInstance&>(*this->pipe),
								  RCFUNCTOR);
	epoll.addCallback(this->pipe->asFd(), READ, cbr.cast<Callback>());
	epoll.addCallback(this->pipe->asFd(), HANGUP, cbh.cast<Callback>());
}

CgiOutput::~CgiOutput() {}

void CgiOutput::setFinished() {
	this->finished = true;
	this->res->setBody(this->body.cast<std::istream>(), this->body_size);
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiOutput.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 15:00:28 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/25 18:30:44 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/http/CgiOutput.hpp"
#include <string>
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "lib/StringHelper.hpp"

void CgiOutput::parseBytes() {
	while (!this->buf->empty()) {
		if (this->finished_headers) {
			char data[2048];
			while (!this->buf->empty()) {
				size_t cpy_len = std::min(sizeof(data), this->buf->size());
				for (size_t i = 0; i < cpy_len; i++)
					data[i] = this->buf->at(i);
				this->buf->erase(this->buf->begin(), this->buf->begin() + cpy_len);
				this->body->write(data, cpy_len);
				this->body_size += cpy_len;
			}
		} else {
			std::string lines(this->buf->begin(), this->buf->end());
			if (!lines.empty())
				LOG(debug, "line = '" << lines << "'");
			Rc<Response> r = this->res;
			while (true) {
				std::string::size_type crlf = lines.find(CRLF);
				if (crlf == std::string::npos)
					return;
				std::string l(lines.begin(), lines.begin() + crlf);
				this->buf->erase(this->buf->begin(), this->buf->begin() + crlf + 2);
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
CgiOutput::CgiOutput(Rc<PipeCgi> p, Rc<Response>& r) : res(r), cgi(p), buf(p->getBuf()) {
	this->body_size		   = 0;
	this->finished_headers = false;
}
CgiOutput::~CgiOutput() {}

void CgiOutput::setFinished() {
	this->finished = true;
	this->res->setBody(this->body.cast<std::istream>(), this->body_size);
};

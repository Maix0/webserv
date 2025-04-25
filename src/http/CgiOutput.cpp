/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiOutput.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 15:00:28 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/25 18:18:26 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/http/CgiOutput.hpp"
#include <string>
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "lib/StringHelper.hpp"

bool CgiOutput::canRead() {
	if (this->can_read)
		return true;
	static bool print_zero = false;
	if (this->buf->size() == 0) {
		if (!print_zero)
			LOG(info, (&*this->buf) << " " << this->buf->size());
		print_zero = true;
	} else {
		print_zero = false;
		LOG(info, (&*this->buf) << " " << this->buf->size());
	}
	std::string lines(this->buf->begin(), this->buf->end());
	if (!lines.empty())
		LOG(debug, "line = '" << lines << "'");
	Rc<Response> r = this->res;
	while (true) {
		std::string::size_type crlf = lines.find(CRLF);
		if (crlf == std::string::npos)
			return false;
		std::string l(lines.begin(), lines.begin() + crlf);
		this->buf->erase(this->buf->begin(), this->buf->begin() + crlf + 2);
		lines.erase(lines.begin(), lines.begin() + crlf + 2);
		if (l.empty()) {
			this->can_read = true;
			return true;
		}
		LOG(info, "cgi_header: '" << l << "'");
		std::string::size_type pos = l.find(':');
		if (pos == std::string::npos) {
			LOG(warn, "invalid reponse from cgi: '" << l << "'");
			continue;
		}
		std::string name(l.begin(), l.begin() + pos);
		std::string val(l.begin() + pos + 1, l.end());
		string_tolower(name);
		string_trim(name);
		string_trim(val);
		r->setHeader(name, val);
	}
	return this->can_read;
}
CgiOutput::CgiOutput(Rc<PipeCgi> p, Rc<Response>& r)
	: res(r), cgi(p), buf(p->getBuf()), can_read(false) {}
CgiOutput::~CgiOutput() {}

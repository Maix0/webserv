/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiOutput.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 15:00:28 by maiboyer          #+#    #+#             */
/*   Updated: 2025/05/02 20:51:41 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>

#include <sys/wait.h>
#include <unistd.h>
#include "app/State.hpp"
#include "app/http/CgiOutput.hpp"
#include "app/http/MimesTypes.hpp"
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "app/http/Routing.hpp"
#include "app/net/Connection.hpp"
#include "config/Config.hpp"
#include "lib/ExitError.hpp"
#include "lib/Functors.hpp"
#include "lib/Rc.hpp"
#include "lib/StringHelper.hpp"

void CgiOutput::parseBytes() {
	while (!this->buffer.empty()) {
		if (this->finished_headers) {
			char data[2048];
			while (!this->buffer.empty()) {
				LOG(info, "trying to copy the remaining " << this->buffer.size() << " bytes");
				size_t cpy_len = std::min(sizeof(data), this->buffer.size());
				for (size_t i = 0; i < cpy_len; i++)
					data[i] = this->buffer.at(i);
				LOG(info, "copied " << cpy_len << " bytes");
				this->buffer.erase(this->buffer.begin(), this->buffer.begin() + cpy_len);
				this->body->write(data, cpy_len);
				this->body_size += cpy_len;
			}
		} else {
			std::string			  lines(this->buffer.begin(), this->buffer.end());
			Option<Rc<Response> > ores = this->res.upgrade();
			if (!ores.hasValue()) {
				this->finished = true;
				return;
			}
			Rc<Response> r = ores.get();
			while (true) {
				std::string::size_type crlf = lines.find(CRLF);
				if (crlf == std::string::npos)
					return;
				std::string l(lines.begin(), lines.begin() + crlf);
				this->buffer.erase(this->buffer.begin(), this->buffer.begin() + crlf + 2);
				lines.erase(lines.begin(), lines.begin() + crlf + 2);
				if (l.empty()) {
					this->finished_headers = true;
					LOG(info, "finished headers for CGI");
					break;
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
				if (name == "status") {
					std::stringstream sval(val);
					int				  c = 0;
					sval >> c;
					r->setStatus(c);
				}
			}
		}
	}
}
CgiOutput::CgiOutput(Epoll&				epoll,
					 Rc<Request>&		req,
					 const config::Cgi* cgi,
					 std::string&		cgi_suffix,
					 Rc<Response>&		res,
					 Rc<Connection>&	conn)
	: conn(conn), res(res), req(req), req_fd(-1), pid(-1) {
	(void)(epoll);
	(void)(req);
	this->body_size					= 0;
	this->finished					= false;
	this->finished_headers			= false;
	const config::Route* rte		= req->getRoute();

	this->script_path				= "";
	this->path_info					= "";
	std::vector<std::string> parts	= url_to_parts(req->getUrl());
	size_t					 r_size = rte->parts.size();
	for (size_t i = 0; i < r_size; i++) {
		this->script_path += "/";
		this->script_path += parts[i];
	}
	size_t i = r_size;
	for (; i < parts.size(); i++) {
		this->script_path += "/";
		this->script_path += parts[i];
		if (string_ends_with(parts[i], cgi_suffix))
			break;
	}
	i++;
	for (; i < parts.size(); i++) {
		this->path_info += "/";
		this->path_info += parts[i];
	}

	if (this->path_info.empty())
		this->path_info = "/";
	else
		this->path_info.erase(0, 0);

	std::string cgi_bin = cgi->binary;
	this->do_exec(cgi_bin);
}

CgiOutput::~CgiOutput() {
	if (this->req_fd != -1)
		close(this->req_fd);
	if (this->pid != -1)
		waitpid(this->pid, NULL, 0);
}

void CgiOutput::setFinished() {
	char buffer[4096];
	this->raw_buf->seekg(0, std::ios::beg);
	{
		int size;
		int dest = open("./cgi_dump", O_WRONLY | O_CREAT | O_TRUNC, 0644);
		lseek(this->raw_buf->getFd(), 0, SEEK_SET);
		while ((size = read(this->raw_buf->getFd(), buffer, sizeof(buffer))) > 0) {
			(void)!write(dest, buffer, size);
		}
		close(dest);
	}
	do {
		errno = 0;
		this->raw_buf->read(buffer, sizeof(buffer));
		int serr = errno;
		(void)(serr);
		this->buffer.insert(this->buffer.end(), &buffer[0], &buffer[this->raw_buf->gcount()]);
		this->parseBytes();
	} while (!(this->raw_buf->eof() || this->raw_buf->fail()));

	this->finished = true;
	this->body->seekg(0, std::ios::beg);
	Option<Rc<Response> > ores = this->res.upgrade();
	if (ores.hasValue()) {
		ores.get()->setBody(this->body.cast<std::istream>(), this->body_size);
	}
};

void CgiOutput::do_exec(std::string& bin) {
	LOG(info, "new CGI");
	Option<Rc<Request> > oreq = this->req.upgrade();
	if (oreq.hasValue() && oreq.get()->getBody().hasValue()) {
		_ERR_RET_THROW(this->req_fd = dup(oreq.get()->getBody().get()->getFd()));
	} else {
		_ERR_RET_THROW(this->req_fd = open("/dev/null", O_RDONLY | O_CLOEXEC));
	}

	_ERR_RET_THROW(fcntl(this->req_fd, FD_CLOEXEC));
	this->pid = 0;

	_ERR_RET_THROW(this->pid = fork());

	if (this->pid == 0) {
		::log::setInsideChild();
		int reserve;
		_ERR_RET_THROW(reserve = dup(STDOUT_FILENO));
		_ERR_RET_THROW(fcntl(reserve, FD_CLOEXEC));

		try {
			std::vector<std::string> sbuf;
			std::vector<char const*> obuf;
			char* const*			 envp = setup_env(State::getInstance().getEnv(), sbuf, obuf);
			char*					 argv[2];
			argv[0] = (char*)(bin.c_str());
			argv[1] = NULL;

			_ERR_RET_THROW(dup2(this->req_fd, STDIN_FILENO));
			_ERR_RET_THROW(dup2(this->raw_buf->getFd(), STDOUT_FILENO));
			_ERR_RET_THROW(execve(bin.c_str(), argv, envp));
		} catch (const std::exception& e) {
			dup2(reserve, STDOUT_FILENO);
			LOG(fatal, "Child threw: " << e.what());
		} catch (...) {
			dup2(reserve, STDOUT_FILENO);
			LOG(fatal, "Child threw unknown exception");
		}
		close(reserve);
		throw ExitError(127);
	}
	close(this->req_fd);
	this->req_fd								  = -1;
	IndexMap<pid_t, ChildStatus>& all_childs_pids = State::getInstance().getChildStatus();
	all_childs_pids.insert(std::make_pair(this->pid, ChildStatus()));
}

#define ADD_HEADER(name, val)       \
	do {                            \
		std::stringstream ss;       \
		ss << (name) << "=" << val; \
		buf.push_back(ss.str());    \
	} while (0);

char* const* CgiOutput ::setup_env(char**					 envp,
								   std::vector<std::string>& buf,
								   std::vector<const char*>& out) {
	std::size_t i			  = 0;

	Option<Rc<Request> > oreq = this->req.upgrade();
	if (!oreq.hasValue())
		throw std::runtime_error("Request died...");
	Rc<Request> req = oreq.get();

	std::string query_string;
	{
		std::string::size_type last_slash = req->getUrl().find('/');
		std::string::size_type first_qmark =
			req->getUrl().find('?', last_slash != std::string::npos ? last_slash : 0);
		if (first_qmark != std::string::npos)
			query_string =
				std::string(req->getUrl().begin() + first_qmark + 1, req->getUrl().end());
	}
	std::string content_type = mime::MimeType::from_extension("binary").getInner();
	if (req->getHeaders().count("content-type"))
		content_type = req->getHeaders()["content-type"];

	ADD_HEADER("AUTH_TYPE", "");
	ADD_HEADER("CONTENT_LENGTH", req->getBodySize());
	ADD_HEADER("CONTENT_TYPE", content_type);
	ADD_HEADER("GATEWAY_INTERFACE", "CGI/1.1");
	ADD_HEADER("PATH_INFO", this->path_info);
	ADD_HEADER("PATH_TRANSLATED", "");
	ADD_HEADER("QUERY_STRING", query_string);
	ADD_HEADER("REMOTE_ADDR", req->getIp());
	ADD_HEADER("REMOTE_PORT", req->getPort());
	ADD_HEADER("REQUEST_METHOD", req->getMethod());
	ADD_HEADER("REQUEST_SCHEME", "http");
	ADD_HEADER("REQUEST_URI", req->getUrl());
	ADD_HEADER("SCRIPT_NAME", this->script_path);
	ADD_HEADER("SERVER_NAME", "localhost");
	ADD_HEADER("SERVER_PORT", req->getServer()->port);
	ADD_HEADER("SERVER_PROTOCOL", "HTTP/1.1");
	ADD_HEADER("SERVER_SOFTWARE", SERVER_NAME << "/0.1");

	Request::HeaderMap& h = req->getHeaders();
	for (Request::HeaderMap::iterator it = h.begin(); it != h.end(); it++) {
		std::string hname("HTTP_");
		hname += it->first;
		string_toupper(hname);
		for (std::string::iterator sit = hname.begin(); sit != hname.end(); sit++) {
			if (*sit == '-')
				*sit = '_';
		}
		ADD_HEADER(hname, it->second);
	}

	(void)(envp);
	// while (envp[i])
	//	env.push_back(envp[i++]);

	for (i = 0; i < buf.size(); i++) {
		out.push_back(buf[i].c_str());
	}
	out.push_back(NULL);
	for (size_t i = 0; i < out.size(); i++) {
		if (out[i] != NULL)
			LOG(debug, "env[" << i << "](" << strlen(out[i]) << ") = " << out[i]);
	}

	return (char* const*)out.data();
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 17:48:13 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/28 01:44:10 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

#include "app/State.hpp"
#include "app/http/CgiOutput.hpp"
#include "app/http/MimesTypes.hpp"
#include "app/http/Response.hpp"
#include "lib/ExitError.hpp"
#include "runtime/Logger.hpp"

#define ADD_HEADER(name, val)       \
	do {                            \
		std::stringstream ss;       \
		ss << (name) << "=" << val; \
		env.push_back(ss.str());    \
	} while (0);

char* const* CgiOutput::PipeInstance::setup_env(char** envp) {
	std::size_t i = 0;

	std::vector<std::string> env;
	ADD_HEADER("CONTENT_LENGTH", this->req->getBodySize());
	ADD_HEADER("GATEWAY_INTERFACE", "CGI/1.1");
	ADD_HEADER("REMOTE_ADDR", this->req->getIp());
	ADD_HEADER("REMOTE_PORT", this->req->getPort());
	ADD_HEADER("REQUEST_METHOD", this->req->getMethod());
	ADD_HEADER("REQUEST_URI", this->req->getUrl());
	ADD_HEADER("REQUEST_SCHEME", "http");
	ADD_HEADER("SERVER_NAME", SERVER_NAME);
	ADD_HEADER("SERVER_PORT", this->req->getServer()->port);
	ADD_HEADER("SERVER_PROTOCOL", "HTTP/1.1");
	ADD_HEADER("SERVER_SOFTWARE", SERVER_NAME << "/1");

	ADD_HEADER("SCRIPT_NAME",
			   std::string(this->req->getUrl().begin(),
						   (this->req->getUrl().find('?') == std::string::npos)
							   ? this->req->getUrl().end()
							   : (this->req->getUrl().begin() + this->req->getUrl().find('?'))));
	{
		std::string content_type = mime::MimeType::from_extension("binary").getInner();
		if (this->req->getHeaders().count("content-type"))
			content_type = this->req->getHeaders()["content-type"];
		ADD_HEADER("CONTENT_TYPE", content_type);
	}

	if (this->req->getUrl().find('?') == std::string::npos) {
		ADD_HEADER("QUERY_STRING",
				   std::string(this->req->getUrl().begin() + this->req->getUrl().find('?') + 1,
							   this->req->getUrl().end()));
	} else
		ADD_HEADER("QUERY_STRING", "");

	while (envp[i])
		env.push_back(envp[i++]);

	char** out = new char*[env.size() + 1];

	for (i = 0; i < env.size(); i++) {
		out[i]		  = new char[env[i].size() + 1];
		std::size_t j = 0;
		for (j = 0; j < env[i].size(); j++)
			out[i][j] = env[i].c_str()[j];
		out[i][j] = '\0';
	}

	out[i] = NULL;
	return const_cast<char* const*>(out);
}

CgiOutput::PipeInstance::PipeInstance(std::string bin, Rc<Request> req, Rc<CgiOutput> parent)
	: parent(parent), req(req), pid(-1), rfd(-1), bin(bin) {
	LOG(info, "new CGI");
	if (this->req->getBody().hasValue())
		this->rfd = this->req->getBody().get()->getFd();
	else
		this->rfd = open("/dev/null", O_RDONLY | O_CLOEXEC);

	this->pid = 0;
	int pip[2];
	_ERR_RET_THROW(::pipe(pip));

	_ERR_RET_THROW(fcntl(pip[0], FD_CLOEXEC));
	_ERR_RET_THROW(fcntl(pip[1], FD_CLOEXEC));

	_ERR_RET_THROW(this->pid = fork());

	if (this->pid == 0) {
		::log::setInsideChild();
		int reserve;
		_ERR_RET_THROW(reserve = dup(STDOUT_FILENO));
		_ERR_RET_THROW(fcntl(reserve, FD_CLOEXEC));

		try {
			char* const* envp = setup_env(State::getInstance().getEnv());
			char*		 argv[2];
			argv[0] = (char*)(this->bin.c_str());
			argv[1] = NULL;

			_ERR_RET_THROW(dup2(this->rfd, STDIN_FILENO));
			_ERR_RET_THROW(dup2(pip[1], STDOUT_FILENO));
			_ERR_RET_THROW(execve(this->bin.c_str(), argv, envp));
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
	close(pip[1]);
	close(rfd);
	this->rfd = pip[0];

	{
		int flags;
		_ERR_RET_THROW((flags = fcntl(this->rfd, F_GETFL)));
		_ERR_RET_THROW(fcntl(this->rfd, F_SETFL, flags | O_NONBLOCK));
	}
}

CgiOutput::PipeInstance::~PipeInstance() {
	if (this->rfd != -1)
		;
	close(this->rfd);
	if (this->pid != -1)
		while (waitpid(this->pid, NULL, 0) != -1)
			;
}

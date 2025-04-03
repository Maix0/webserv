/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 17:16:21 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 19:25:37 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/http/Request.hpp"
#include "app/State.hpp"
#include "app/http/Routing.hpp"
#include "app/http/StatusCode.hpp"
#include "config/Config.hpp"
#include "lib/IndexMap.hpp"
#include "lib/StringHelper.hpp"
#include "runtime/Logger.hpp"

#include <fcntl.h>
#include <sys/types.h>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <sstream>
#include <string>

using std::string;
using std::vector;

static const vector<string> _all_multiheaders() {
	vector<string> out;

	out.push_back("cookie");

	return out;
}

const vector<string>						  Request::ALLOWED_MULTIHEADERS = _all_multiheaders();
typedef IndexMap<std::string, config::Server> ServerMap;

bool Request::parseBytes(std::string& buffer) {
	assert(MAX_HEADERS_SIZE > MAX_URI_SIZE);
	bool continue_loop = false;
	do {
		continue_loop = true;
		switch (this->state) {
			case Request::HEADER: {
				std::string::size_type crlf = buffer.find(CRLF);

				if (crlf == std::string::npos) {
					if (buffer.size() >= MAX_URI_SIZE)
						throw PageException(status::URI_TOO_LONG);
					return false;
				}
				if (crlf + 2 >= MAX_URI_SIZE)
					throw PageException(status::URI_TOO_LONG);
				this->headers_total_size += crlf + 2;

				std::stringstream first_line(string(buffer.begin(), buffer.begin() + crlf));
				buffer.erase(buffer.begin(), buffer.begin() + crlf + 2);

				string method, path, version;

				std::getline(first_line, method, ' ');
				std::getline(first_line, path, ' ');
				std::getline(first_line, version, ' ');

				if (method.empty() || path.empty() || version.empty()) {
					throw PageException(status::BAD_REQUEST);
				}
				this->state = Request::USERHEADERS;

				break;
			};
			case Request::USERHEADERS: {
				std::string::size_type end	= buffer.find(CRLF CRLF);
				std::string::size_type crlf = buffer.find(CRLF);
				if (crlf == std::string::npos) {
					if (this->headers_total_size + buffer.size() >= MAX_HEADERS_SIZE)
						throw PageException(status::REQUEST_HEADER_FIELDS_TOO_LARGE);
					return false;
				}
				if (this->headers_total_size + crlf + 2 >= MAX_HEADERS_SIZE)
					throw PageException(status::REQUEST_HEADER_FIELDS_TOO_LARGE);
				this->headers_total_size += crlf + 2;

				std::string header_line(string(buffer.begin(), buffer.begin() + crlf));
				buffer.erase(buffer.begin(), buffer.begin() + crlf + 2);

				string::size_type delim = header_line.find(":");
				string			  name(header_line.begin(), header_line.begin() + delim);
				string			  value(header_line.begin() + delim + 1, header_line.end());

				string_trim(name);
				string_trim(value);

				for (string::iterator it = name.begin(); it != name.end(); it++)
					*it = std::tolower(*it);

				if (this->headers.count(name) == 0) {
					this->headers.insert(std::make_pair(name, value));
				} else {
					/// we check that it is a valid header to be concatenated.
					/// either it is in a premade list, or it is a Vendor-specific header (start
					/// with `X-`)
					/// if either condition is true, then proceed to concat. otherwise just throw
					/// 400
					if (!(string_start_with(name, "X-") ||
						  std::find(ALLOWED_MULTIHEADERS.begin(), ALLOWED_MULTIHEADERS.end(),
									name) != ALLOWED_MULTIHEADERS.end())) {
						throw PageException(status::BAD_REQUEST);
					}
					this->headers.at(name) += string(",") + value;
				}

				if (end == crlf) {
					this->state		= BODY;
					this->body_size = 0;
					buffer.erase(buffer.begin(), buffer.begin() + 2);
					continue_loop = true;

					if (this->headers.count("host")) {
						std::string host = this->headers.at("host");
						host.erase(host.begin() + host.find(":"), host.end());

						ServerMap& servers = State::getInstance().getConfig().server;
						for (ServerMap::iterator it = servers.begin(); it != servers.end(); it++) {
							LOG(info, "this->Port = " << this->port << "; Host='" << host
													  << "'; Server->Port = " << it->second.port
													  << "; Server->name = '" << it->second.hostname
													  << "'");
							if (it->second.port == this->port) {
								this->server = &it->second;
								break;
							}
						}

						for (ServerMap::iterator it = servers.begin(); it != servers.end(); it++) {
							if (it->second.port == this->port && it->second.hostname.hasValue() &&
								it->second.hostname.get() == host) {
								this->server = &it->second;
								break;
							}
						}

						assert(this->server != NULL);
						this->route = getRouteFor(*this->server, this->url);
					}
					if (this->headers.count("host")) {
						char* end = NULL;
						errno	  = 0;
						unsigned long long val =
							std::strtoull(this->headers.at("host").c_str(), &end, 10);
						if (errno != 0 || (end != NULL && *end != '\0'))
							throw PageException(400);
						this->content_length = val;
					}
					if (this->route && this->content_length != -1 &&
						(size_t)this->content_length > this->route->max_size)
						throw PageException(status::PAYLOAD_TOO_LARGE);
				}
				break;
			};
			case Request::BODY: {
				if (this->body_fd == -1)
					_ERR_RET_THROW(this->body_fd = open("/tmp", O_RDWR | O_CLOEXEC | O_TMPFILE, 0));
				size_t size = buffer.size();
				if (this->content_length != -1 &&
					(ssize_t)(this->body_size + buffer.size()) > this->content_length) {
					size = this->content_length + buffer.size() - this->content_length;
				}

				this->body_size += size;
				_ERR_RET_THROW(write(this->body_fd, buffer.data(), buffer.size()));
				buffer.erase(buffer.begin(), buffer.begin() + size);

				if ((ssize_t)this->body_size >= this->content_length) {
					this->state = Request::FINISHED;
					return true;
				}
				break;
			}

			case Request::UNDEFINED: {
				_UNREACHABLE;
			}
			case Request::FINISHED: {
				_UNREACHABLE;
			}
		}
	} while (!buffer.empty() || continue_loop);
	return false;
}

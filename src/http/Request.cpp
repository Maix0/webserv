/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 17:16:21 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/24 16:15:11 by maiboyer         ###   ########.fr       */
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
		continue_loop = false;
		LOG(debug, "Request state: " << Request::state_to_str(this->state));
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
				LOG(debug, "first_line = '" << first_line.str() << "'");
				buffer.erase(buffer.begin(), buffer.begin() + crlf + 2);

				string method, path, version;

				std::getline(first_line, method, ' ');
				std::getline(first_line, path, ' ');
				std::getline(first_line, version, ' ');

				if (method.empty() || path.empty() || version.empty()) {
					LOG(fatal, "hello ?????");
					throw PageException(status::BAD_REQUEST);
				}
				if (version != "HTTP/1.1")
					throw PageException(status::BAD_REQUEST);
				this->method = method;
				this->url	 = path;
				this->state	 = Request::USERHEADERS;

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
				LOG(debug, "headerline = '" << header_line << "'");
				buffer.erase(buffer.begin(), buffer.begin() + crlf + 2);

				if (!header_line.empty()) {
					string::size_type delim = header_line.find(":");
					string			  name(header_line.begin(), header_line.begin() + delim);
					string			  value(header_line.begin() + delim + 1, header_line.end());

					string_trim(name);
					string_trim(value);
					string_tolower(name);

					if (this->headers.count(name) == 0) {
						this->headers.insert(std::make_pair(name, value));
					} else {
						/// we check that it is a valid header to be concatenated.
						/// either it is in a premade list, or it is a Vendor-specific header (start
						/// with `x-`)
						/// if either condition is true, then proceed to concat. otherwise just
						/// throw 400
						if (!(string_start_with(name, "x-") ||
							  std::find(ALLOWED_MULTIHEADERS.begin(), ALLOWED_MULTIHEADERS.end(),
										name) != ALLOWED_MULTIHEADERS.end())) {
							throw PageException(status::BAD_REQUEST);
						}
						this->headers.at(name) += string(",") + value;
					}
				}

				if (header_line.empty() || end == crlf) {
					this->state		= PREBODY;
					this->body_size = 0;
					continue_loop	= true;
				}
				break;
			};
			case Request::PREBODY: {
				// we need to remove the first CRLF
				if (buffer.size() < 2) {
					LOG(info, "we need more data... ;" << buffer.size() << " '" << buffer << "'");
					return false;
				}
				// we remove the first CRLF
				buffer.erase(buffer.begin(), buffer.begin() + 2);

				if (!(this->method == "POST" || this->method == "PUT")) {
					continue_loop = true;
					this->state	  = FINISHED;
				} else
					this->state = BODY;

				// if the host header is set, then use it to find the Server
				if (this->headers.count("host")) {
					std::string host = this->headers.at("host");
					host.erase(host.begin() + host.find(":"), host.end());

					ServerMap& servers = State::getInstance().getConfig().server;
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

				// we may have chunked BODY, handle that
				if (this->headers.count("transfer-encoding")) {
					if (this->headers.at("transfer-encoding") == "chunked") {
						this->state = Request::CHUNKED_BODY_HEADER;
					} else {
					}
				}
				// if the content-length is set, then read it
				if (this->headers.count("content-length")) {
					// content-length can't be used with chunked body...
					if (this->state == Request::CHUNKED_BODY_HEADER)
						throw PageException(status::BAD_REQUEST);
					char* end = NULL;
					errno	  = 0;
					unsigned long long val =
						std::strtoull(this->headers.at("content-length").c_str(), &end, 10);
					if (errno != 0 || (end != NULL && *end != '\0'))
						throw PageException(400);
					this->content_length = val;
				}

				if (this->route && this->content_length != -1 &&
					(size_t)this->content_length > this->route->max_size)
					throw PageException(status::PAYLOAD_TOO_LARGE);
				break;
			}

			case Request::CHUNKED_BODY_HEADER: {
				std::string::size_type crlf = buffer.find(CRLF);
				if (crlf == std::string::npos)
					return false;
				std::string header(buffer.begin(), buffer.begin() + crlf);
				buffer.erase(buffer.begin(), buffer.begin() + crlf + 2);

				char* end			   = NULL;
				errno				   = 0;
				unsigned long long val = std::strtoull(header.c_str(), &end, 16);
				if (errno != 0 || (end != NULL && *end != '\0'))
					throw PageException(400);
				this->current_chunk_size   = val;
				this->remaining_chunk_size = val;
				this->state				   = Request::CHUNKED_BODY_NEW_CHUNK;

				break;
			};
			case Request::CHUNKED_BODY_NEW_CHUNK: {
				if (this->current_chunk_size == 0)
					this->state = Request::CHUNKED_BODY_DONE;
				else
					this->state = Request::CHUNKED_BODY_READ_CHUNK;
				break;
			};
			case Request::CHUNKED_BODY_READ_CHUNK: {
				size_t		to_read = std::min(buffer.size(), this->remaining_chunk_size);
				std::string data(buffer.begin(), buffer.begin() + to_read);
				buffer.erase(buffer.begin(), buffer.begin() + to_read);

				assert(this->remaining_chunk_size >= to_read);
				this->body_size			   += to_read;
				this->remaining_chunk_size -= to_read;

				if (this->remaining_chunk_size == 0)
					this->state = CHUNKED_BODY_END_CHUNK;
				else
					this->state = CHUNKED_BODY_READ_CHUNK;

				break;
			};
			case Request::CHUNKED_BODY_END_CHUNK: {
				if (buffer.size() < 2)
					return false;
				assert(buffer.find(CRLF) == 0);
				buffer.erase(buffer.begin(), buffer.begin() + 2);
				this->state = Request::CHUNKED_BODY_HEADER;
				break;
			};
			case Request::CHUNKED_BODY_DONE: {
				LOG(info, "buf='" << buffer << "'");
				if (buffer.size() < 2) {
					LOG(debug, "need more data...");
					return false;
				}
				assert(buffer.find(CRLF) == 0);
				buffer.erase(buffer.begin(), buffer.begin() + 2);
				this->state	  = Request::FINISHED;
				continue_loop = true;
				break;
			};

			case Request::BODY: {
				size_t size = buffer.size();

				if (this->content_length != -1 &&
					(ssize_t)(this->body_size + buffer.size()) > this->content_length) {
					size = this->content_length + buffer.size() - this->content_length;
				}

				this->body_size += size;
				this->body.get_or_insert()->write(buffer.data(), size);
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
				LOG(debug, "finished with body_size = " << this->body_size);
				return true;
			}
		}
	} while (!buffer.empty() || continue_loop);
	return false;
}

static const std::pair<Request::ParsingState, std::string> _state_to_str_inner[] = {
	std::make_pair(Request::UNDEFINED, "UNDEFINED"),
	std::make_pair(Request::HEADER, "HEADER"),
	std::make_pair(Request::USERHEADERS, "USERHEADERS"),
	std::make_pair(Request::PREBODY, "PREBODY"),
	std::make_pair(Request::BODY, "BODY"),
	std::make_pair(Request::CHUNKED_BODY_HEADER, "CHUNKED_BODY_HEADER"),
	std::make_pair(Request::CHUNKED_BODY_NEW_CHUNK, "CHUNKED_BODY_NEW_CHUNK"),
	std::make_pair(Request::CHUNKED_BODY_READ_CHUNK, "CHUNKED_BODY_READ_CHUNK"),
	std::make_pair(Request::CHUNKED_BODY_END_CHUNK, "CHUNKED_BODY_END_CHUNK"),
	std::make_pair(Request::CHUNKED_BODY_DONE, "CHUNKED_BODY_DONE"),
	std::make_pair(Request::FINISHED, "FINISHED"),
};

const std::map<Request::ParsingState, std::string> Request::STATE_TO_STR(
	&_state_to_str_inner[0],
	&_state_to_str_inner[sizeof(_state_to_str_inner) / sizeof(_state_to_str_inner[0])]);

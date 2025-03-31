/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 17:16:21 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/31 17:57:13 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Request.hpp"
#include <fcntl.h>
#include "app/Logger.hpp"
#include "app/StringHelper.hpp"

#include <cctype>
#include <sstream>
#include <string>

using std::string;
using std::vector;

static const vector<string> _all_multiheaders() {
	vector<string> out;

	out.push_back("cookie");

	return out;
}

const vector<string> Request::ALLOWED_MULTIHEADERS = _all_multiheaders();

void Request::parseBytes(vector<char>::const_iterator beg, vector<char>::const_iterator end) {
	this->tmp_buffer.insert(this->tmp_buffer.end(), beg, end);
	do {
		switch (this->state) {
			case Request::HEADER: {
				std::string::size_type clrf = this->tmp_buffer.find(CLRF);
				if (clrf == std::string::npos)
					return;
				std::stringstream first_line(
					string(this->tmp_buffer.begin(), this->tmp_buffer.begin() + clrf));
				this->tmp_buffer.erase(this->tmp_buffer.begin(),
									   this->tmp_buffer.begin() + clrf + 2);
				LOG(debug, "MainHeader: " << first_line);
				string method, path, version;

				std::getline(first_line, method, ' ');
				std::getline(first_line, path, ' ');
				std::getline(first_line, version, ' ');

				if (method.empty() || path.empty() || version.empty()) {
					throw PageException() return;
				}
				this->state = Request::USERHEADERS;
				break;
			};
			case Request::USERHEADERS: {
				std::string::size_type clrf = this->tmp_buffer.find(CLRF);
				if (clrf == std::string::npos)
					return;
				std::string header_line(this->tmp_buffer.begin(), this->tmp_buffer.begin() + clrf);
				this->tmp_buffer.erase(this->tmp_buffer.begin(),
									   this->tmp_buffer.begin() + clrf + 2);
				if (header_line.empty()) {
					this->state = Request::BODY;
					break;
				}
				string::size_type delim = header_line.find(":");
				string			  name(header_line.begin(), header_line.begin() + delim);
				string			  value(header_line.begin() + delim + 1, header_line.end());
				trim(name);
				trim(value);
				for (string::iterator it = name.begin(); it != name.end(); it++)
					*it = std::tolower(*it);

				if (this->headers.count(name) == 0) {
					this->headers.insert(std::make_pair(name, value));
				} else {
					if (std::find(Request::ALLOWED_MULTIHEADERS.begin(),
								  Request::ALLOWED_MULTIHEADERS.end(),
								  name) == Request::ALLOWED_MULTIHEADERS.end()) {
						// throw
						return;
					}
					this->headers.at(name) += string(",") + value;
				}
				LOG(debug, "HttpHeader: " << name << ": " << value);

				break;
			};
			case Request::BODY: {
				if (this->body_fd != -1)
					_ERR_RET_THROW(this->body_fd = open("/tmp", O_RDWR | O_CLOEXEC | O_TMPFILE));
				size_t size = this->tmp_buffer.size();
				if (this->content_length != -1 &&
					(ssize_t)(this->body_size + this->tmp_buffer.size()) > this->content_length) {
					size = this->content_length + this->tmp_buffer.size() - this->content_length;
				}

				this->body_size += size;
				_ERR_RET_THROW(
					write(this->body_fd, this->tmp_buffer.data(), this->tmp_buffer.size()));
				this->tmp_buffer.erase(this->tmp_buffer.begin(), this->tmp_buffer.begin() + size);
				if (this->content_length == this->body_size) {
					this->state = Request::FINISHED;
				}
			}

			case Request::UNDEFINED: {
				_UNREACHABLE;
			}
			case Request::FINISHED: {
				_UNREACHABLE;
			}
		}
	} while (!this->tmp_buffer.empty());
}

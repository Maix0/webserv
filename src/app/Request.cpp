/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 17:16:21 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/29 18:06:14 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app/Request.hpp"
#include <string>
#include "app/Logger.hpp"

using std::string;
using std::vector;

static const vector<string> _all_multiheaders() {
	vector<string> out;

	out.push_back("cookie");

	return out;
}

const std::vector<std::string> Request::ALLOWED_MULTIHEADERS = _all_multiheaders();

void Request::parseBytes(std::vector<char>::const_iterator beg,
						 std::vector<char>::const_iterator end) {
	this->tmp_buffer.insert(this->tmp_buffer.end(), beg, end);
	do {
		switch (this->state) {
			case Request::HEADER: {
				std::string::size_type clrf = this->tmp_buffer.find(CLRF);
				if (clrf == std::string::npos)
					return;
				std::string first_line(this->tmp_buffer.begin(), this->tmp_buffer.begin() + clrf);
				this->tmp_buffer.erase(this->tmp_buffer.begin(), this->tmp_buffer.begin() + clrf + 2);
				LOG(debug, "MainHeader: " << first_line);
				this->state = Request::USERHEADERS;

				break;
			};
			case Request::USERHEADERS: {
				std::string::size_type clrf = this->tmp_buffer.find(CLRF);
				if (clrf == std::string::npos)
					return;
				std::string header_line(this->tmp_buffer.begin(), this->tmp_buffer.begin() + clrf);
				this->tmp_buffer.erase(this->tmp_buffer.begin(), this->tmp_buffer.begin() + clrf + 2);
				if (header_line.empty()) {
					this->state = Request::BODY;
					break;
				}
				LOG(debug, "HttpHeader: " << header_line);

				break;
			};
			case Request::BODY: {
				if (!this->body.hasValue())
					this->body.insert(std::string());
				this->body.get().insert(this->body.get().end(), this->tmp_buffer.begin(),
										this->tmp_buffer.end());
				break;
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

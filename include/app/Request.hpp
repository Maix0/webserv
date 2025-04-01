/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:51:48 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/01 17:01:00 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <sstream>
#include <string>
#include <vector>
#include "app/IndexMap.hpp"
#include "app/Logger.hpp"

#define CRLF "\r\n"

struct StatusCode {
	private:
		int _code;

	public:
		StatusCode(int code) : _code(code) { assert(100 <= code && code < 600); }
		~StatusCode() {};
		StatusCode(const StatusCode& rhs) : _code(rhs._code) {};
		StatusCode& operator=(const StatusCode& rhs) {
			if (this != &rhs)
				this->_code = rhs._code;
			return (*this);
		}

		std::string canonical() const;
		int			code() const { return this->_code; };
};

class Request {
	public:
		typedef IndexMap<std::string, std::string> HeaderMap;
		typedef std::string						   Url;
		typedef std::string						   Method;

		static const std::vector<std::string> ALLOWED_MULTIHEADERS;
		static const std::size_t			  MAX_URI_SIZE	   = 1 << 10;
		static const std::size_t			  MAX_HEADERS_SIZE = 1 << 16;

		static std::string createStatusPageFor(StatusCode code);

		enum ParsingState { UNDEFINED, HEADER, USERHEADERS, BODY, FINISHED };

	private:
		HeaderMap	 headers;
		Url			 url;
		Method		 method;
		ParsingState state;
		size_t		 headers_total_size;

		// POST DATA FOR BODIES
		int		body_fd;
		size_t	body_size;
		ssize_t content_length;

	public:
		const HeaderMap& getHeaders() const { return this->headers; };
		const Url&		 getUrl() const { return this->url; };
		const Method&	 getMethod() const { return this->method; };

		HeaderMap&	 getHeaders() { return this->headers; };
		Url&		 getUrl() { return this->url; };
		Method&		 getMethod() { return this->method; };
		ParsingState getState() { return this->state; };

		void parseBytes(std::string& buffer);

		Request() : state(HEADER), body_fd(-1), body_size(-1) {};
		~Request() {
			if (this->body_fd != -1)
				close(this->body_fd);
		};
		void setFinished() { this->state = FINISHED; };

		class PageException : public std::exception {
			private:
				std::string str;
				int			code;

			public:
				virtual ~PageException() throw() {}
				PageException(int code) : code(code) {
					std::stringstream ss;
					ss << "requested Page for status code " << code;
					this->str = ss.str();
				}
				virtual const char* what(void) const throw() { return this->str.c_str(); }
				int					statusCode() const { return this->code; };
		};
};

#define STATUS_CODE(CODE, NAME, CANONICAL)            \
	static inline StatusCode __##CODE() throw() {     \
		assert(0 <= CODE && CODE < 600);              \
		_STATUS_NAMES[CODE] = CANONICAL;              \
		return StatusCode(CODE);                      \
	}                                                 \
	static const StatusCode	 NAME		= __##CODE(); \
	static const std::string STR_##NAME = CANONICAL;

namespace status {

	static const char* _STATUS_NAMES[600] = {};

	/// 100 Continue

	/// [[RFC9110,
	/// Section 15.2.1](https://datatracker.ietf.org/doc/html/rfc9110#section-15.2.1)]

	STATUS_CODE(100, CONTINUE, "Continue");

	/// 101 Switching Protocols

	/// [[RFC9110,
	/// Section 15.2.2](https://datatracker.ietf.org/doc/html/rfc9110#section-15.2.2)]

	STATUS_CODE(101, SWITCHING_PROTOCOLS, "Switching Protocols");

	/// 102 Processing

	/// [[RFC2518,
	/// Section 10.1](https://datatracker.ietf.org/doc/html/rfc2518#section-10.1)]

	STATUS_CODE(102, PROCESSING, "Processing");

	/// 200 OK

	/// [[RFC9110,
	/// Section 15.3.1](https://datatracker.ietf.org/doc/html/rfc9110#section-15.3.1)]

	STATUS_CODE(200, OK, "OK");

	/// 201 Created

	/// [[RFC9110,
	/// Section 15.3.2](https://datatracker.ietf.org/doc/html/rfc9110#section-15.3.2)]

	STATUS_CODE(201, CREATED, "Created");

	/// 202 Accepted

	/// [[RFC9110,
	/// Section 15.3.3](https://datatracker.ietf.org/doc/html/rfc9110#section-15.3.3)]

	STATUS_CODE(202, ACCEPTED, "Accepted");

	/// 203 Non-Authoritative Information

	/// [[RFC9110,
	/// Section 15.3.4](https://datatracker.ietf.org/doc/html/rfc9110#section-15.3.4)]

	STATUS_CODE(203, NON_AUTHORITATIVE_INFORMATION, "Non Authoritative Information");

	/// 204 No Content

	/// [[RFC9110,
	/// Section 15.3.5](https://datatracker.ietf.org/doc/html/rfc9110#section-15.3.5)]

	STATUS_CODE(204, NO_CONTENT, "No Content");

	/// 205 Reset Content

	/// [[RFC9110,
	/// Section 15.3.6](https://datatracker.ietf.org/doc/html/rfc9110#section-15.3.6)]

	STATUS_CODE(205, RESET_CONTENT, "Reset Content");

	/// 206 Partial Content

	/// [[RFC9110,
	/// Section 15.3.7](https://datatracker.ietf.org/doc/html/rfc9110#section-15.3.7)]

	STATUS_CODE(206, PARTIAL_CONTENT, "Partial Content");

	/// 207 Multi-Status

	/// [[RFC4918,
	/// Section 11.1](https://datatracker.ietf.org/doc/html/rfc4918#section-11.1)]

	STATUS_CODE(207, MULTI_STATUS, "Multi-Status");

	/// 208 Already Reported

	/// [[RFC5842, Section 7.1](https://datatracker.ietf.org/doc/html/rfc5842#section-7.1)]

	STATUS_CODE(208, ALREADY_REPORTED, "Already Reported");

	/// 226 IM Used

	/// [[RFC3229,
	/// Section 10.4.1](https://datatracker.ietf.org/doc/html/rfc3229#section-10.4.1)]

	STATUS_CODE(226, IM_USED, "IM Used");

	/// 300 Multiple Choices

	/// [[RFC9110,
	/// Section 15.4.1](https://datatracker.ietf.org/doc/html/rfc9110#section-15.4.1)]

	STATUS_CODE(300, MULTIPLE_CHOICES, "Multiple Choices");

	/// 301 Moved Permanently

	/// [[RFC9110,
	/// Section 15.4.2](https://datatracker.ietf.org/doc/html/rfc9110#section-15.4.2)]

	STATUS_CODE(301, MOVED_PERMANENTLY, "Moved Permanently");

	/// 302 Found

	/// [[RFC9110,
	/// Section 15.4.3](https://datatracker.ietf.org/doc/html/rfc9110#section-15.4.3)]

	STATUS_CODE(302, FOUND, "Found");

	/// 303 See Other

	/// [[RFC9110,
	/// Section 15.4.4](https://datatracker.ietf.org/doc/html/rfc9110#section-15.4.4)]

	STATUS_CODE(303, SEE_OTHER, "See Other");

	/// 304 Not Modified

	/// [[RFC9110,
	/// Section 15.4.5](https://datatracker.ietf.org/doc/html/rfc9110#section-15.4.5)]

	STATUS_CODE(304, NOT_MODIFIED, "Not Modified");

	/// 305 Use Proxy

	/// [[RFC9110,
	/// Section 15.4.6](https://datatracker.ietf.org/doc/html/rfc9110#section-15.4.6)]

	STATUS_CODE(305, USE_PROXY, "Use Proxy");

	/// 307 Temporary Redirect

	/// [[RFC9110,
	/// Section 15.4.7](https://datatracker.ietf.org/doc/html/rfc9110#section-15.4.7)]

	STATUS_CODE(307, TEMPORARY_REDIRECT, "Temporary Redirect");

	/// 308 Permanent Redirect

	/// [[RFC9110,
	/// Section 15.4.8](https://datatracker.ietf.org/doc/html/rfc9110#section-15.4.8)]

	STATUS_CODE(308, PERMANENT_REDIRECT, "Permanent Redirect");

	/// 400 Bad Request

	/// [[RFC9110,
	/// Section 15.5.1](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.1)]

	STATUS_CODE(400, BAD_REQUEST, "Bad Request");

	/// 401 Unauthorized

	/// [[RFC9110,
	/// Section 15.5.2](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.2)]

	STATUS_CODE(401, UNAUTHORIZED, "Unauthorized");

	/// 402 Payment Required

	/// [[RFC9110,
	/// Section 15.5.3](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.3)]

	STATUS_CODE(402, PAYMENT_REQUIRED, "Payment Required");

	/// 403 Forbidden

	/// [[RFC9110,
	/// Section 15.5.4](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.4)]

	STATUS_CODE(403, FORBIDDEN, "Forbidden");

	/// 404 Not Found

	/// [[RFC9110,
	/// Section 15.5.5](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.5)]

	STATUS_CODE(404, NOT_FOUND, "Not Found");

	/// 405 Method Not Allowed

	/// [[RFC9110,
	/// Section 15.5.6](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.6)]

	STATUS_CODE(405, METHOD_NOT_ALLOWED, "Method Not Allowed");

	/// 406 Not Acceptable

	/// [[RFC9110,
	/// Section 15.5.7](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.7)]

	STATUS_CODE(406, NOT_ACCEPTABLE, "Not Acceptable");

	/// 407 Proxy Authentication Required

	/// [[RFC9110,
	/// Section 15.5.8](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.8)]

	STATUS_CODE(407, PROXY_AUTHENTICATION_REQUIRED, "Proxy Authentication Required");

	/// 408 Request Timeout

	/// [[RFC9110,
	/// Section 15.5.9](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.9)]

	STATUS_CODE(408, REQUEST_TIMEOUT, "Request Timeout");

	/// 409 Conflict

	/// [[RFC9110,
	/// Section 15.5.10](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.10)]

	STATUS_CODE(409, CONFLICT, "Conflict");

	/// 410 Gone

	/// [[RFC9110,
	/// Section 15.5.11](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.11)]

	STATUS_CODE(410, GONE, "Gone");

	/// 411 Length Required

	/// [[RFC9110,
	/// Section 15.5.12](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.12)]

	STATUS_CODE(411, LENGTH_REQUIRED, "Length Required");

	/// 412 Precondition Failed

	/// [[RFC9110,
	/// Section 15.5.13](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.13)]

	STATUS_CODE(412, PRECONDITION_FAILED, "Precondition Failed");

	/// 413 Payload Too Large

	/// [[RFC9110,
	/// Section 15.5.14](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.14)]

	STATUS_CODE(413, PAYLOAD_TOO_LARGE, "Payload Too Large");

	/// 414 URI Too Long

	/// [[RFC9110,
	/// Section 15.5.15](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.15)]

	STATUS_CODE(414, URI_TOO_LONG, "URI Too Long");

	/// 415 Unsupported Media Type

	/// [[RFC9110,
	/// Section 15.5.16](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.16)]

	STATUS_CODE(415, UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type");

	/// 416 Range Not Satisfiable

	/// [[RFC9110,
	/// Section 15.5.17](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.17)]

	STATUS_CODE(416, RANGE_NOT_SATISFIABLE, "Range Not Satisfiable");

	/// 417 Expectation Failed

	/// [[RFC9110,
	/// Section 15.5.18](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.18)]

	STATUS_CODE(417, EXPECTATION_FAILED, "Expectation Failed");

	/// 418 I'm a teapot

	/// [curiously not registered by IANA but [RFC2324,
	/// Section 2.3.2](https://datatracker.ietf.org/doc/html/rfc2324#section-2.3.2)]

	STATUS_CODE(418, IM_A_TEAPOT, "I'm a teapot");

	/// 421 Misdirected Request

	/// [[RFC9110,
	/// Section 15.5.20](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.20)]

	STATUS_CODE(421, MISDIRECTED_REQUEST, "Misdirected Request");

	/// 422 Unprocessable Entity

	/// [[RFC9110,
	/// Section 15.5.21](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.21)]

	STATUS_CODE(422, UNPROCESSABLE_ENTITY, "Unprocessable Entity");

	/// 423 Locked

	/// [[RFC4918,
	/// Section 11.3](https://datatracker.ietf.org/doc/html/rfc4918#section-11.3)]

	STATUS_CODE(423, LOCKED, "Locked");

	/// 424 Failed Dependency

	/// [[RFC4918, Section 11.4](https://tools.ietf.org/html/rfc4918#section-11.4)]

	STATUS_CODE(424, FAILED_DEPENDENCY, "Failed Dependency");

	/// 425 Too early

	/// [[RFC8470, Section 5.2](https://httpwg.org/specs/rfc8470.html#status)]

	STATUS_CODE(425, TOO_EARLY, "Too Early");

	/// 426 Upgrade Required

	/// [[RFC9110,
	/// Section 15.5.22](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.22)]

	STATUS_CODE(426, UPGRADE_REQUIRED, "Upgrade Required");

	/// 428 Precondition Required

	/// [[RFC6585, Section 3](https://datatracker.ietf.org/doc/html/rfc6585#section-3)]

	STATUS_CODE(428, PRECONDITION_REQUIRED, "Precondition Required");

	/// 429 Too Many Requests

	/// [[RFC6585, Section 4](https://datatracker.ietf.org/doc/html/rfc6585#section-4)]

	STATUS_CODE(429, TOO_MANY_REQUESTS, "Too Many Requests");

	/// 431 Request Header Fields Too Large

	/// [[RFC6585, Section 5](https://datatracker.ietf.org/doc/html/rfc6585#section-5)]

	STATUS_CODE(431, REQUEST_HEADER_FIELDS_TOO_LARGE, "Request Header Fields Too Large");

	/// 451 Unavailable For Legal Reasons

	/// [[RFC7725, Section 3](https://tools.ietf.org/html/rfc7725#section-3)]

	STATUS_CODE(451, UNAVAILABLE_FOR_LEGAL_REASONS, "Unavailable For Legal Reasons");

	/// 500 Internal Server Error

	/// [[RFC9110,
	/// Section 15.6.1](https://datatracker.ietf.org/doc/html/rfc9110#section-15.6.1)]

	STATUS_CODE(500, INTERNAL_SERVER_ERROR, "Internal Server Error");

	/// 501 Not Implemented

	/// [[RFC9110,
	/// Section 15.6.2](https://datatracker.ietf.org/doc/html/rfc9110#section-15.6.2)]

	STATUS_CODE(501, NOT_IMPLEMENTED, "Not Implemented");

	/// 502 Bad Gateway

	/// [[RFC9110,
	/// Section 15.6.3](https://datatracker.ietf.org/doc/html/rfc9110#section-15.6.3)]

	STATUS_CODE(502, BAD_GATEWAY, "Bad Gateway");

	/// 503 Service Unavailable

	/// [[RFC9110,
	/// Section 15.6.4](https://datatracker.ietf.org/doc/html/rfc9110#section-15.6.4)]

	STATUS_CODE(503, SERVICE_UNAVAILABLE, "Service Unavailable");

	/// 504 Gateway Timeout

	/// [[RFC9110,
	/// Section 15.6.5](https://datatracker.ietf.org/doc/html/rfc9110#section-15.6.5)]

	STATUS_CODE(504, GATEWAY_TIMEOUT, "Gateway Timeout");

	/// 505 HTTP Version Not Supported

	/// [[RFC9110,
	/// Section 15.6.6](https://datatracker.ietf.org/doc/html/rfc9110#section-15.6.6)]

	STATUS_CODE(505, HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported");

	/// 506 Variant Also Negotiates

	/// [[RFC2295, Section 8.1](https://datatracker.ietf.org/doc/html/rfc2295#section-8.1)]

	STATUS_CODE(506, VARIANT_ALSO_NEGOTIATES, "Variant Also Negotiates");

	/// 507 Insufficient Storage

	/// [[RFC4918,
	/// Section 11.5](https://datatracker.ietf.org/doc/html/rfc4918#section-11.5)]

	STATUS_CODE(507, INSUFFICIENT_STORAGE, "Insufficient Storage");

	/// 508 Loop Detected

	/// [[RFC5842, Section 7.2](https://datatracker.ietf.org/doc/html/rfc5842#section-7.2)]

	STATUS_CODE(508, LOOP_DETECTED, "Loop Detected");

	/// 510 Not Extended

	/// [[RFC2774, Section 7](https://datatracker.ietf.org/doc/html/rfc2774#section-7)]

	STATUS_CODE(510, NOT_EXTENDED, "Not Extended");

	/// 511 Network Authentication Required

	/// [[RFC6585, Section 6](https://datatracker.ietf.org/doc/html/rfc6585#section-6)]

	STATUS_CODE(511, NETWORK_AUTHENTICATION_REQUIRED, "Network Authentication Required");
}  // namespace status

#undef STATUS_CODE

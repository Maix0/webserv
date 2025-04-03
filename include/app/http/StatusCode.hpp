/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusCode.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 15:40:51 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/03 13:11:41 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cassert>
#include <map>
#include <string>

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

#define STATUS_CODE_VAL(CODE, NAME, CANONICAL) static const StatusCode NAME = StatusCode(CODE);
#define STATUS_CODE_STR(CODE, NAME, CANONICAL) std::make_pair(CODE, CANONICAL),
#define STATUS_CODE_MAP(CODE, NAME, CANONICAL) std::make_pair(CODE, NAME),
#define STATUS_CODE(TY, CODE, NAME, CANONICAL) STATUS_CODE_##TY(CODE, NAME, CANONICAL)

#define STATUS_CODES_ALL(TY)                                                                 \
	STATUS_CODE(TY, 100, CONTINUE, "Continue")                                               \
	STATUS_CODE(TY, 101, SWITCHING_PROTOCOLS, "Switching Protocols")                         \
	STATUS_CODE(TY, 102, PROCESSING, "Processing")                                           \
	STATUS_CODE(TY, 200, OK, "OK")                                                           \
	STATUS_CODE(TY, 201, CREATED, "Created")                                                 \
	STATUS_CODE(TY, 202, ACCEPTED, "Accepted")                                               \
	STATUS_CODE(TY, 203, NON_AUTHORITATIVE_INFORMATION, "Non Authoritative Information")     \
	STATUS_CODE(TY, 204, NO_CONTENT, "No Content")                                           \
	STATUS_CODE(TY, 205, RESET_CONTENT, "Reset Content")                                     \
	STATUS_CODE(TY, 206, PARTIAL_CONTENT, "Partial Content")                                 \
	STATUS_CODE(TY, 207, MULTI_STATUS, "Multi-Status")                                       \
	STATUS_CODE(TY, 208, ALREADY_REPORTED, "Already Reported")                               \
	STATUS_CODE(TY, 226, IM_USED, "IM Used")                                                 \
	STATUS_CODE(TY, 300, MULTIPLE_CHOICES, "Multiple Choices")                               \
	STATUS_CODE(TY, 301, MOVED_PERMANENTLY, "Moved Permanently")                             \
	STATUS_CODE(TY, 302, FOUND, "Found")                                                     \
	STATUS_CODE(TY, 303, SEE_OTHER, "See Other")                                             \
	STATUS_CODE(TY, 304, NOT_MODIFIED, "Not Modified")                                       \
	STATUS_CODE(TY, 305, USE_PROXY, "Use Proxy")                                             \
	STATUS_CODE(TY, 307, TEMPORARY_REDIRECT, "Temporary Redirect")                           \
	STATUS_CODE(TY, 308, PERMANENT_REDIRECT, "Permanent Redirect")                           \
	STATUS_CODE(TY, 400, BAD_REQUEST, "Bad Request")                                         \
	STATUS_CODE(TY, 401, UNAUTHORIZED, "Unauthorized")                                       \
	STATUS_CODE(TY, 402, PAYMENT_REQUIRED, "Payment Required")                               \
	STATUS_CODE(TY, 403, FORBIDDEN, "Forbidden")                                             \
	STATUS_CODE(TY, 404, NOT_FOUND, "Not Found")                                             \
	STATUS_CODE(TY, 405, METHOD_NOT_ALLOWED, "Method Not Allowed")                           \
	STATUS_CODE(TY, 406, NOT_ACCEPTABLE, "Not Acceptable")                                   \
	STATUS_CODE(TY, 407, PROXY_AUTHENTICATION_REQUIRED, "Proxy Authentication Required")     \
	STATUS_CODE(TY, 408, REQUEST_TIMEOUT, "Request Timeout")                                 \
	STATUS_CODE(TY, 409, CONFLICT, "Conflict")                                               \
	STATUS_CODE(TY, 410, GONE, "Gone")                                                       \
	STATUS_CODE(TY, 411, LENGTH_REQUIRED, "Length Required")                                 \
	STATUS_CODE(TY, 412, PRECONDITION_FAILED, "Precondition Failed")                         \
	STATUS_CODE(TY, 413, PAYLOAD_TOO_LARGE, "Payload Too Large")                             \
	STATUS_CODE(TY, 414, URI_TOO_LONG, "URI Too Long")                                       \
	STATUS_CODE(TY, 415, UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type")                   \
	STATUS_CODE(TY, 416, RANGE_NOT_SATISFIABLE, "Range Not Satisfiable")                     \
	STATUS_CODE(TY, 417, EXPECTATION_FAILED, "Expectation Failed")                           \
	STATUS_CODE(TY, 418, IM_A_TEAPOT, "I'm a teapot")                                        \
	STATUS_CODE(TY, 421, MISDIRECTED_REQUEST, "Misdirected Request")                         \
	STATUS_CODE(TY, 422, UNPROCESSABLE_ENTITY, "Unprocessable Entity")                       \
	STATUS_CODE(TY, 423, LOCKED, "Locked")                                                   \
	STATUS_CODE(TY, 424, FAILED_DEPENDENCY, "Failed Dependency")                             \
	STATUS_CODE(TY, 425, TOO_EARLY, "Too Early")                                             \
	STATUS_CODE(TY, 426, UPGRADE_REQUIRED, "Upgrade Required")                               \
	STATUS_CODE(TY, 428, PRECONDITION_REQUIRED, "Precondition Required")                     \
	STATUS_CODE(TY, 429, TOO_MANY_REQUESTS, "Too Many Requests")                             \
	STATUS_CODE(TY, 431, REQUEST_HEADER_FIELDS_TOO_LARGE, "Request Header Fields Too Large") \
	STATUS_CODE(TY, 451, UNAVAILABLE_FOR_LEGAL_REASONS, "Unavailable For Legal Reasons")     \
	STATUS_CODE(TY, 500, INTERNAL_SERVER_ERROR, "Internal Server Error")                     \
	STATUS_CODE(TY, 501, NOT_IMPLEMENTED, "Not Implemented")                                 \
	STATUS_CODE(TY, 502, BAD_GATEWAY, "Bad Gateway")                                         \
	STATUS_CODE(TY, 503, SERVICE_UNAVAILABLE, "Service Unavailable")                         \
	STATUS_CODE(TY, 504, GATEWAY_TIMEOUT, "Gateway Timeout")                                 \
	STATUS_CODE(TY, 505, HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported")           \
	STATUS_CODE(TY, 506, VARIANT_ALSO_NEGOTIATES, "Variant Also Negotiates")                 \
	STATUS_CODE(TY, 507, INSUFFICIENT_STORAGE, "Insufficient Storage")                       \
	STATUS_CODE(TY, 508, LOOP_DETECTED, "Loop Detected")                                     \
	STATUS_CODE(TY, 510, NOT_EXTENDED, "Not Extended")                                       \
	STATUS_CODE(TY, 511, NETWORK_AUTHENTICATION_REQUIRED, "Network Authentication Required")

namespace status {

	STATUS_CODES_ALL(VAL);
	static const std::pair<int, const char*> _STATUS_CODES_STR[] = {STATUS_CODES_ALL(STR)};
	static const std::pair<int, StatusCode>	 _STATUS_CODES_MAP[] = {STATUS_CODES_ALL(MAP)};

	static const std::map<int, std::string> _NAMES(
		&_STATUS_CODES_STR[0],
		&_STATUS_CODES_STR[sizeof(_STATUS_CODES_STR) / sizeof(_STATUS_CODES_STR[0])]);
	static const std::map<int, StatusCode> _VALUES(
		&_STATUS_CODES_MAP[0],
		&_STATUS_CODES_MAP[sizeof(_STATUS_CODES_MAP) / sizeof(_STATUS_CODES_MAP[0])]);

}  // namespace status

#undef STATUS_CODE
#undef STATUS_CODE_MAP
#undef STATUS_CODE_STR
#undef STATUS_CODE_VAL

#undef STATUS_CODES_ALL

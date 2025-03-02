/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 14:33:11 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/02 17:17:05 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "app/Option.hpp"
#include "toml/TomlValue.hpp"

struct Cgi {
	/// @default `false`
	bool		from_env;

	/// @required
	/// if (from_env) then binary is used as an ENV_VAR that contains the path to the cgi binary
	/// else: the path to the cgi binary
	std::string binary;

	static Cgi	fromTomlValue(const TomlValue& toml);
};

struct Route {
	/// @default 'Option.None()'
	/// Option.None() => everything is allowed
	/// Option.Some() => only the matched method are allowed
	Option<std::vector<std::string> >  allowed;

	/// @default 'Option.None()'
	/// Option.None() => No index set
	/// Option.Some() => try to find the index file
	Option<std::string>				   index;

	/// @default 'Option.None()'
	/// Option.None() => do not every try to do cgi
	/// Option.Some() => try to match extension and lookup CGI handler from the global table
	std::map<std::string, std::string> cgi;

	/// @default `true`
	/// true => show the directory listing if no index is found when hitting a directory url
	/// false => show the 403 listing if no index is found when hitting a directory url;
	/// if index == Option.Some() then 404
	bool							   listing;

	/// @default `16_000_000` (16MB)
	std::size_t						   max_size;

	/// @default 'Option.None()'
	/// Option.None() => Post are just recieved, and either passed to cgi or ignored
	/// Option.Some() => Post are written to that directory (if content.size() < max_size). May also
	/// be passed to cgi
	Option<std::string>				   post_dir;

	/// @default `Option.None()`
	/// Option.None() => inherit the root of the server
	/// Option.Some() => set the root when searching for files
	Option<std::string>				   root;

	/// @default `Option.None()`
	/// Option.None() => no redirect
	/// Option.Some() => redirect to said url
	Option<std::string>				   redirect;

	static Route					   fromTomlValue(const TomlValue& toml);
};

struct Server {
	/// @required
	std::string						   root;
	/// @required
	std::vector<unsigned short>		   ports;
	/// @required
	std::string						   host;

	std::map<std::string, Route>	   routes;

	/// Map error code to files (relative to `root`)
	std::map<std::string, std::string> errors;

	Option<std::string>				   servername;

	static Server					   fromTomlValue(const TomlValue& toml);
};

struct Config {
	std::map<std::string, Cgi>	  cgi;
	std::map<std::string, Server> server;

	static Config				  fromTomlValue(const TomlValue& toml);
};

std::ostream& operator<<(std::ostream&, const Config&);
std::ostream& operator<<(std::ostream&, const Server&);
std::ostream& operator<<(std::ostream&, const Route&);
std::ostream& operator<<(std::ostream&, const Cgi&);

#define ERROR(NAME)                                 \
	class NAME##Error : public std::exception {     \
	private:                                        \
		std::string msg;                            \
                                                    \
	public:                                         \
		NAME##Error& operator=(const NAME##Error&); \
		NAME##Error();                              \
		NAME##Error(const NAME##Error&);            \
		NAME##Error(const std::string& msg);        \
		virtual const char* what() const throw();   \
		virtual ~NAME##Error() throw();             \
	};

ERROR(CgiParse);
ERROR(ConfigParse);
ERROR(RouteParse);
ERROR(ServerParse);

#undef ERROR

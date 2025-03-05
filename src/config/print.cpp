/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 17:01:09 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/05 16:58:25 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <iostream>
#include <ostream>
#include "config/Config.hpp"

std::ostream& _print_ident(std::ostream& o, std::size_t ident) {
	while (ident--)
		o << "\t";
	return (o);
}

void _print_cgi(std::ostream& o, const Cgi& cgi, std::size_t ident) {
	o << "{" << std::endl;
	_print_ident(o, ident + 1) << "binary   => " << cgi.binary << std::endl;
	_print_ident(o, ident + 1) << "from_env => " << (cgi.from_env ? "true" : "false") << std::endl;
	_print_ident(o, ident) << "}";
}
void _print_route(std::ostream& o, const Route& route, std::size_t ident) {
	bool first = true;
	o << "{" << std::endl;
	ident++;

	_print_ident(o, ident) << "root     => " << route.root << "," << std::endl;
	_print_ident(o, ident) << "index    => " << route.index << "," << std::endl;
	_print_ident(o, ident) << "listing  => " << (route.listing ? "true" : "false") << ","
						   << std::endl;
	_print_ident(o, ident) << "max_size => " << route.max_size << "," << std::endl;
	_print_ident(o, ident) << "post_dir => " << route.post_dir << "," << std::endl;
	_print_ident(o, ident) << "redirect => " << route.redirect << "," << std::endl;
	if (route.allowed.hasValue()) {
		_print_ident(o, ident) << "allowed  => Some({ ";
		for (std::vector<std::string>::const_iterator it = route.allowed.get().begin();
			 it != route.allowed.get().end(); it++) {
			if (!first)
				first = false, o << ", ";
			o << *it << " ";
		}
		o << "})," << std::endl;
	} else
		_print_ident(o, ident) << "allowed   => None," << std::endl;
	first = true;
	_print_ident(o, ident) << "cgi      => {" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = route.cgi.begin();
		 it != route.cgi.end(); it++) {
		if (!first)
			first = false, o << ", " << std::endl;
		_print_ident(o, ident + 1) << it->first << " => " << it->second;
	}
	o << std::endl;
	_print_ident(o, ident) << "}" << std::endl;
	_print_ident(o, ident - 1) << "}";
}
void _print_server(std::ostream& o, const Server& server, std::size_t ident) {
	bool first = true;
	o << "{" << std::endl;
	ident++;
	_print_ident(o, ident) << "root   => " << server.root << "," << std::endl;
	first = true;
	_print_ident(o, ident) << "errors => {" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = server.errors.begin();
		 it != server.errors.end(); it++) {
		if (!first)
			first = false, o << ", " << std::endl;
		_print_ident(o, ident + 1) << it->first << " => " << it->second;
	}
	o << std::endl;
	_print_ident(o, ident) << "}," << std::endl;
	_print_ident(o, ident) << "routes => {" << std::endl;
	for (std::map<std::string, Route>::const_iterator it = server.routes.begin();
		 it != server.routes.end(); it++) {
		if (!first)
			first = false, o << ", " << std::endl;
		_print_ident(o, ident + 1) << it->first << " => ";
		_print_route(o, it->second, ident + 1);
	}
	o << std::endl;
	_print_ident(o, ident) << "}" << std::endl;
	_print_ident(o, ident - 1) << "}";
}

void _print_listener(std::ostream& o, const Listener& server, std::size_t ident) {
	_print_ident(o, ident++) << "{" << std::endl;
	_print_ident(o, ident) << "host   => " << server.host << "," << std::endl;

	bool first = true;
	_print_ident(o, ident) << "port   => [ ";
	for (std::vector<Port>::const_iterator it = server.port.begin(); it != server.port.end();
		 it++) {
		if (!first)
			first = false, o << ", ";
		o << *it << " ";
	}
	o << "]" << std::endl;
	_print_ident(o, ident - 1) << "}" << std::endl;
}

std::ostream& operator<<(std::ostream& o, const Listener& e) {
	_print_listener(o, e, 0);
	return (o);
}
std::ostream& operator<<(std::ostream& o, const Config& e) {
	std::size_t ident = 1;
	bool		first = true;
	o << "{" << std::endl;
	_print_ident(o, ident) << "cgi    => {" << std::endl;
	for (std::map<std::string, Cgi>::const_iterator it = e.cgi.begin(); it != e.cgi.end(); it++) {
		if (!first)
			o << ",\n";
		first = false;
		_print_ident(o, ident + 1) << it->first << " => ";
		_print_cgi(o, it->second, ident + 1);
	}
	o << std::endl;
	_print_ident(o, ident) << "}, " << std::endl;

	first = true;
	_print_ident(o, ident) << "server => {" << std::endl;
	for (std::map<std::string, Server>::const_iterator it = e.server.begin(); it != e.server.end();
		 it++) {
		if (!first)
			o << ",\n";
		first = false;
		_print_ident(o, ident + 1) << it->first << " => ";
		_print_server(o, it->second, ident + 1);
	}
	o << std::endl;
	_print_ident(o, ident) << "}" << std::endl;

	_print_ident(o, ident) << "listener => {" << std::endl;
	for (std::map<std::string, Listener>::const_iterator it = e.listener.begin();
		 it != e.listener.end(); it++) {
		if (!first)
			o << ",\n";
		first = false;
		_print_ident(o, ident + 1) << it->first << " => ";
		_print_listener(o, it->second, ident + 1);
	}
	o << std::endl;
	_print_ident(o, ident) << "}" << std::endl;
	o << "}" << std::endl;

	return o;
}

std::ostream& operator<<(std::ostream& o, const Server& e) {
	_print_server(o, e, 0);
	return (o);
}
std::ostream& operator<<(std::ostream& o, const Route& e) {
	_print_route(o, e, 0);
	return (o);
}
std::ostream& operator<<(std::ostream& o, const Cgi& e) {
	_print_cgi(o, e, 0);
	return (o);
}

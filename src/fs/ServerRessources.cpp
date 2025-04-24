/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRessources.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:21:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/24 22:17:55 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>
#include <cstddef>
#include <ctime>
#include <fstream>
#include <ios>
#include <istream>
#include <sstream>
#include <string>

#include "app/fs/Directory.hpp"
#include "app/fs/ServerRessources.hpp"
#include "app/http/Request.hpp"
#include "app/http/Response.hpp"
#include "config/Config.hpp"
#include "lib/Rc.hpp"
#include "lib/StringHelper.hpp"

using std::string;

std::string get_last_modified(struct stat& s) {
	char	   buffer[1024]	 = {};
	struct tm* last_modified = localtime(&s.st_mtim.tv_sec);

	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", last_modified);
	return buffer;
}

std::string get_created_at_modified(struct stat& s) {
	char	   buffer[1024]	 = {};
	struct tm* last_modified = localtime(&s.st_ctim.tv_sec);

	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", last_modified);
	return buffer;
}

Rc<std::istream> getFileAt(const std::string&	 path,
						   const config::Server* server,
						   const config::Route*	 route,
						   std::string*			 extension,
						   std::size_t*			 body_size,
						   Response::HeaderMap*	 extraHeader) {
	std::string			dummy;
	std::size_t			dummy2;
	Response::HeaderMap dummy3;
	if (extension == NULL)
		extension = &dummy;
	if (body_size == NULL)
		body_size = &dummy2;
	if (extraHeader == NULL)
		extraHeader = &dummy3;

	std::string real_path = "/";
	if (server == NULL) {
		real_path  = "./";
		real_path += path;
	} else {
		std::string root = server->root;
		if (route != NULL && route->root.hasValue())
			root = route->root.get();
		real_path = root + "/" + path;
	}

	for (std::string::size_type pos = real_path.find("//"); pos != std::string::npos;
		 pos						= real_path.find("//"))
		   real_path.replace(pos, 2, "/");

	struct stat s;
	if (stat(real_path.c_str(), &s) == 0) {
		if (S_ISDIR(s.st_mode)) {
			try {
				if (route != NULL && route->index.hasValue()) {
					real_path += "/" + route->index.get();
					return getFileAt(real_path, NULL, NULL, extension, body_size, extraHeader);
				}
			} catch (const fs::error::NotFound& _e) {
			} catch (const fs::error::IsADirectory& _e) {
			}
			if (route != NULL && route->listing) {
				if (!(s.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
					throw fs::error::NotAllowed(real_path);
				LOG(info, "Directory listing for " << real_path);
				Directory dir(real_path);
				*extension = "html";
				Rc<std::stringstream> out;
				(*out) << "<html>" CRLF;

				(*out) << "<head>" CRLF;
				(*out) << "\t<title>" << "Directory listing - " << string_escape_html(dir.getPath())
					   << "</title>" << CRLF;
				(*out) << "</head>" CRLF;

				(*out) << "<body>" CRLF;
				(*out) << "\t<h1>" << "Directory listing - " << string_escape_html(dir.getPath())
					   << "</h1>" << CRLF;
				(*out) << "<hr>" << CRLF;
				(*out) << "\t<table style=\"border-spacing: 10px 0; font-familly: monospace\">"
					   << CRLF;
				(*out)
					<< "\t\t<tr><td>Dir</td><td>Name</td><td>Size</td><td>Created at</td><td>Last "
					   "Modified</td></tr>";
				const std::vector<Directory::Entry>& entries = dir.getEntries();
				for (std::vector<Directory::Entry>::const_iterator it = entries.begin();
					 it != entries.end(); it++) {
					bool		has_size  = false;
					bool		has_times = false;
					bool		is_dir	  = false;
					std::size_t size	  = 0;
					std::string created_at;
					std::string f_realpath = real_path + "/" + it->name;
					std::string fullname   = it->name;
					std::string last_modified;
					struct stat s2;
					if (stat(f_realpath.c_str(), &s2) == 0) {
						if (S_ISDIR(s2.st_mode)) {
							is_dir	  = true;
							fullname += "/";
						} else {
							has_size = true;
							size	 = s2.st_size;
						}
						has_times	  = true;
						created_at	  = get_created_at_modified(s2);
						last_modified = get_last_modified(s2);
					}
					(*out) << "\t\t<tr><td>" << (is_dir ? "X" : "") << " </td><td><a href =\""
						   << string_escape_html(fullname) << "\">" << string_escape_html(fullname)
						   << "</a></td>";
					if (has_size)
						(*out) << "<td>" << size << "</td>";
					else
						(*out) << "<td></td>";
					if (has_times)
						(*out) << "<td>" << created_at << "</td><td>" << last_modified << "</td>";
					else
						(*out) << "<td></td>";
					(*out) << "</tr>" << CRLF;
				}
				(*out) << "\t</table>" << CRLF;
				(*out) << "<hr>" << CRLF;
				(*out) << "</body>" CRLF;

				(*out) << "</html>" CRLF;
				*body_size						= out->str().size();
				(*extraHeader)["Last-Modified"] = get_last_modified(s);
				return out.cast<std::istream>();
			}
			throw fs::error::IsADirectory(real_path);
		} else if (S_ISREG(s.st_mode)) {
			if (!(s.st_mode & (S_IRUSR | S_IRGRP | S_IROTH)))
				throw fs::error::NotAllowed(real_path);
			Rc<std::ifstream> file;
			LOG(trace, "trying to open a file" << real_path);
			file->open(real_path.c_str());
			if (file->fail())
				throw fs::error::Failure(real_path);
			(*file) >> std::noskipws;
			*body_size = s.st_size;
			LOG(trace, "we set the size to " << *body_size);

			std::string::size_type last_slash		   = real_path.find_last_of('/');
			std::string::size_type first_dot_last_part = real_path.find_first_of('.', last_slash);
			std::string ext(real_path.begin() + first_dot_last_part + 1, real_path.end());
			*extension = ext;
			return file.cast<std::istream>();
		}
	}
	throw fs::error::NotFound(real_path);
}

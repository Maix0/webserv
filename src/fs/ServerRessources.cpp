/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRessources.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:21:07 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/08 16:02:56 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>

#include "app/fs/Directory.hpp"
#include "app/fs/ServerRessources.hpp"
#include "app/http/Request.hpp"
#include "config/Config.hpp"
#include "lib/StringHelper.hpp"

using std::string;

std::string getFileAt(const std::string& path,
					  config::Server*	 server,
					  config::Route*	 route,
					  std::string*		 extension) {
	std::string dummy;
	if (extension == NULL)
		extension = &dummy;

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

	struct stat s;
	if (stat(real_path.c_str(), &s) == 0) {
		if (S_ISDIR(s.st_mode)) {
			try {
				if (route != NULL && route->index.hasValue()) {
					real_path += "/" + route->index.get();
					return getFileAt(real_path, NULL, NULL, extension);
				}
			} catch (const fs::error::NotFound& _e) {
			} catch (const fs::error::IsADirectory& _e) {
			}
			if (route != NULL && route->listing) {
				if (s.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
					throw fs::error::NotAllowed(real_path);
				Directory dir(real_path);
				*extension = "txt";
				std::stringstream out;
				out << "<html>" CRLF;

				out << "<head>" CRLF;
				out << "\t<title>" << "Directory listing - " << string_escape_html(dir.getPath())
					<< "</title>" << CRLF;
				out << "</head>" CRLF;

				out << "<body>" CRLF;
				out << "\t<h1>" << "Directory listing - " << string_escape_html(dir.getPath())
					<< "</h1>" << CRLF;
				out << "\t<ul>" << CRLF;
				const std::vector<Directory::Entry>& entries = dir.getEntries();
				for (std::vector<Directory::Entry>::const_iterator it = entries.begin();
					 it != entries.end(); it++) {
					out << "\t\t<li><a href=\"" << string_escape_html(it->name) << "\">"
						<< string_escape_html(it->name) << "</a></li>" << CRLF;
				}
				out << "\t</ul>" << CRLF;
				out << "</body>" CRLF;

				out << "</html>" CRLF;
				return out.str();
			}
			throw fs::error::IsADirectory(real_path);
		} else if (S_ISREG(s.st_mode)) {
			if (s.st_mode & (S_IRUSR | S_IRGRP | S_IROTH))
				throw fs::error::NotAllowed(real_path);
			/// TODO: fix this ?
			/// this limits the files to 1GiB max, since it needs to read them from disk AT ONCE,
			/// stalling the entire process while reading...
			if (s.st_size > (1 << 30)) {
				throw fs::error::TooBig(real_path);
			}
			std::ifstream file;
			std::string	  out;
			file.open(real_path.c_str());
			if (file.fail())
				throw fs::error::Failure(real_path);
			file >> std::noskipws;
			file >> out;
			file.close();

			std::string::size_type last_slash		   = real_path.find_last_of('/');
			std::string::size_type first_dot_last_part = real_path.find_first_of('.', last_slash);
			std::string ext(real_path.begin() + first_dot_last_part + 1, real_path.end());
			*extension = ext;
			return out;
		}
	}
	throw fs::error::NotFound(real_path);
}

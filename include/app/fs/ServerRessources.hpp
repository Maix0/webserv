/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRessources.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 13:23:58 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/13 23:52:35 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <istream>
#include <stdexcept>
#include <string>
#include "config/Config.hpp"
#include "lib/Rc.hpp"

// this returns a filedescriptor that contains the file data
Rc<std::istream> getFileAt(const std::string& path,
						   config::Server*	  server,
						   config::Route*	  route = NULL,
						   std::string* extension	= NULL);

namespace fs {
	namespace error {
		struct ErrorBase : public std::runtime_error {
				const std::string file;
				const std::string kind;
				ErrorBase(const std::string& file, const std::string& kind)
					: std::runtime_error(std::string("fs error: " + kind + ":" + file)),
					  file(file),
					  kind(kind) {};
				virtual ~ErrorBase() throw() {};
		};

		namespace kind {
			static const std::string NOT_FOUND		= "Not found";
			static const std::string IS_A_DIRECTORY = "Path is a directory";
			static const std::string TOO_BIG		= "File is too big";
			static const std::string NOT_ALLOWED	= "Operation now allowed";
			static const std::string FAILURE		= "Unknown failure";

		};	// namespace kind

		/*
		struct PlaceHolder : public ErrorBase {
				PlaceHolder(const std::string& file) : ErrorBase(file, kind::PLACEHOLDER) {};
				virtual ~PlaceHolder() throw() {};
		};
		*/
		struct NotFound : public ErrorBase {
				NotFound(const std::string& file) : ErrorBase(file, kind::NOT_FOUND) {};
				virtual ~NotFound() throw() {};
		};

		struct IsADirectory : public ErrorBase {
				IsADirectory(const std::string& file) : ErrorBase(file, kind::IS_A_DIRECTORY) {};
				virtual ~IsADirectory() throw() {};
		};

		struct TooBig : public ErrorBase {
				TooBig(const std::string& file) : ErrorBase(file, kind::TOO_BIG) {};
				virtual ~TooBig() throw() {};
		};
		struct NotAllowed : public ErrorBase {
				NotAllowed(const std::string& file) : ErrorBase(file, kind::NOT_ALLOWED) {};
				virtual ~NotAllowed() throw() {};
		};
		struct Failure : public ErrorBase {
				Failure(const std::string& file) : ErrorBase(file, kind::FAILURE) {};
				virtual ~Failure() throw() {};
		};
	};	// namespace error
}  // namespace fs

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directory.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 14:09:26 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/13 14:30:23 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
namespace app {
	class Directory {
	public:
		struct Entry {
			std::string name;
		};

	private:
		std::vector<Entry> entries;
		std::string		   path;

	public:
		Directory();
		Directory(const std::string& path);

		Directory(const Directory&);
		Directory& operator=(const Directory&);

		~Directory();

		const std::vector<Entry>& getEntries() const { return this->entries; };
		const std::string&		  getPath() const { return this->path; };
	};

};	// namespace app

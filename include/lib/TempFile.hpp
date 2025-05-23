/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TempFile.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:39:31 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/25 15:06:02 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <fstream>
#include <string>

class tiostream : public std::fstream {
	private:
		std::string filename;
		int			fd;

	public:
		tiostream();
		~tiostream();

		const std::string& getFilename() const throw() { return this->filename; };
		int				   getFd() const throw() { return this->fd; };
};

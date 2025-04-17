/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TempFile.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:39:31 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/17 17:04:33 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

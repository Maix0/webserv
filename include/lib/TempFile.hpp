/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TempFile.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 13:39:31 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/15 21:47:11 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>

class tiostream : public std::fstream {
	private:
		std::string filename;

	public:
		tiostream();
		~tiostream();

		const std::string& getFilename() const throw() { return this->filename; };
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Semaphore.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 22:25:22 by maiboyer          #+#    #+#             */
/*   Updated: 2025/04/24 22:35:37 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
class Semaphore {
	private:
		std::string name;
		void*		inner;

	public:
		Semaphore(const std::string& name, std::size_t count = 1);
		~Semaphore();

		class Ticket {
			private:
				Semaphore& parent;

			public:
				Ticket(Semaphore& parent);
				~Ticket();
		};
};

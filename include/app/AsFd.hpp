/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AsFd.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maiboyer <maiboyer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:03:41 by maiboyer          #+#    #+#             */
/*   Updated: 2025/03/25 22:42:18 by maiboyer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Something that can be cast to an Fd
class AsFd {
	public:
		virtual ~AsFd() {};
		virtual int asFd() = 0;
};

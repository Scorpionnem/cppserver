/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Packet.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:35:17 by mbatty            #+#    #+#             */
/*   Updated: 2026/02/05 18:08:54 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
#include <cstdlib>

struct Packet
{
	struct	Header
	{
		size_t	size;
		size_t	id;
	};
	Header	hdr;
};

struct	IntPacket : public Packet
{
	int	v;
};

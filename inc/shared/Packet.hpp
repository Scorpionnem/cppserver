/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Packet.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:35:17 by mbatty            #+#    #+#             */
/*   Updated: 2026/02/06 13:10:28 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <iostream>

struct Packet
{
	struct	Header
	{
		Header() {}
		~Header() {}

		size_t	id;
	};

	Header	hdr;
};

struct	IntPacket : public Packet
{
	static size_t	size() {return (sizeof(IntPacket));}
	static Packet	*create() {return (new IntPacket());}
	int	v;
};

/*
	Packet to send strings, max length of 4096 characters
*/
struct	StringPacket : public Packet
{
	static size_t	size() {return (sizeof(StringPacket));}
	static Packet	*create() {return (new StringPacket());}

	std::string	toString() {return (msg);}
	void		setMsg(const std::string &msg)
	{
		std::memcpy(this->msg, msg.c_str(), std::min(msg.size(), (size_t)4096 - 1));
	}

	char	msg[4096] = {0};
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:27:57 by mbatty            #+#    #+#             */
/*   Updated: 2026/02/06 13:09:23 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Packet.hpp"
#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Server.hpp"
#include <sys/signal.h>

int	g_sig = 0;

void	handleSig(int sig)
{
	g_sig = sig;
}

int	main(void)
{
	signal(SIGINT, handleSig);

	Server	server;
	server.setPacketType(0, IntPacket::size, IntPacket::create);
	server.setPacketType(1, StringPacket::size, StringPacket::create);
	server.setConnectCallback([]
		(const Server::Client &client)
		{
			(void)client;
		});
	server.setDisconnectCallback([]
		(const Server::Client &client)
		{
			(void)client;
		});
	server.setMessageCallback([&server]
		(const Server::Client &client, Packet *packet)
		{
			(void)server;(void)client;
			if (packet->hdr.id == 0)
			{
				IntPacket	*pckt = (IntPacket*)packet;
				std::cout << pckt->v << std::endl;
			}
			if (packet->hdr.id == 1)
			{
				StringPacket	*pckt = (StringPacket*)packet;
				std::cout << pckt->msg << std::endl;
			}
		});

	try {
		server.open(6942);
		while (g_sig == 0)
			server.update();
		server.close();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}

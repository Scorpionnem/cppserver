/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:27:57 by mbatty            #+#    #+#             */
/*   Updated: 2026/02/05 18:08:51 by mbatty           ###   ########.fr       */
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

IntPacket	*intPacketMaker(int fd)
{
	IntPacket	*res = new IntPacket();

	ssize_t	intPacketSize = sizeof(IntPacket) - sizeof(Packet::Header);
	ssize_t	size = recv(fd, reinterpret_cast<char*>(res) + sizeof(Packet::Header), intPacketSize, 0);
	if (size != intPacketSize)
	{
		std::cerr << "Invalid intpacket received" << std::endl;
		delete res;
		return (NULL);
	}
	return (res);
}

int	main(void)
{
	signal(SIGINT, handleSig);

	Server	server;
	server.setPacketType(0, intPacketMaker);
	server.setConnectCallback([]
		(const Server::Client &client)
		{
			std::cout << "Connect callback for client: " << client.fd() << std::endl;
		});
	server.setDisconnectCallback([]
		(const Server::Client &client)
		{
			std::cout << "Disconnect callback for client: " << client.fd() << std::endl;
		});
	server.setMessageCallback([&server]
		(const Server::Client &client, Packet *packet)
		{
			(void)server;
			std::cout << "Message callback for client: " << client.fd() << std::endl;
			if (packet->hdr.id == 0)
			{
				IntPacket	*pckt = (IntPacket*)packet;
				std::cout << pckt->v << std::endl;
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

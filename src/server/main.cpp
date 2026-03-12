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
	server.setConnectCallback([]
		(const Server::Client &client)
		{
			std::cout << "client connected: " << client.fd() << std::endl;
		});
	server.setDisconnectCallback([]
		(const Server::Client &client)
		{
			std::cout << "client disconnected: " << client.fd() << std::endl;
		});
	server.setMessageCallback([&server]
		(const Server::Client &client, Packet *packet)
		{
			(void)server;(void)client;
			if (static_cast<PacketType>(packet->hdr.id) == PacketType::INT)
			{
				IntPacket	*pckt = (IntPacket*)packet;

				std::cout << pckt->v << std::endl;
			}
			if (static_cast<PacketType>(packet->hdr.id) == PacketType::STRING)
			{
				StringPacket	*pckt = (StringPacket*)packet;

				std::cout << pckt->msg << std::endl;
			}
		});

	server.setPacketType(static_cast<int>(PacketType::INT), IntPacket::size, IntPacket::create);
	server.setPacketType(static_cast<int>(PacketType::STRING), StringPacket::size, StringPacket::create);

	try {
		server.open(6942);
		while (g_sig == 0)
			server.update();
		server.close();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}

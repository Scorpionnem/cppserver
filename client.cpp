#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "inc/shared/Packet.hpp"

int main()
{
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(6942);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	connect(clientSocket, (struct sockaddr*)&serverAddress,
			sizeof(serverAddress));

	IntPacket	packet;

	packet.hdr.id = 0;
	packet.v = 6942;
	send(clientSocket, &packet, sizeof(IntPacket), 0);

	StringPacket	spacket;

	spacket.hdr.id = 1;
	spacket.setMsg("message");
	send(clientSocket, &spacket, sizeof(StringPacket), 0);

	close(clientSocket);

	return 0;
}


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
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	connect(clientSocket, (struct sockaddr*)&serverAddress,
			sizeof(serverAddress));

	Packet	packet;

	packet.hdr.id = 1;
	packet.hdr.size = sizeof(Packet);
	send(clientSocket, &packet, sizeof(Packet), 0);

	close(clientSocket);

	return 0;
}

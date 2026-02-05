/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:27:57 by mbatty            #+#    #+#             */
/*   Updated: 2026/02/05 15:57:56 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Packet.hpp"
#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

struct	IntPacket : public Packet
{
	int	v;
};

int main()
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
		std::cout << strerror(errno) << std::endl;

	int	yes = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		close(serverSocket);
		return (0);
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	listen(serverSocket, 5);

	int clientSocket = accept(serverSocket, nullptr, nullptr);

	void	*buffer[4096] = { 0 };
	Packet	*packet = (Packet*)buffer;
	size_t	size = recv(clientSocket, buffer, sizeof(Packet), 0);
	if (size != sizeof(Packet))
	{
		std::cout << "Invalid packet received" << std::endl;
	}
	else
	{
		std::cout << "size " << packet->hdr.size << " id " << packet->hdr.id << std::endl;
		if (packet->hdr.id == 1)
		{
			if (packet->hdr.size != sizeof(IntPacket))
				std::cout << "Wrong intpacket size" << std::endl;
			else
				std::cout << "Intpacket" << std::endl;
		}
	}

	close(serverSocket);
	return (0);
}

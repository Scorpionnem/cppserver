/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 17:28:35 by mbatty            #+#    #+#             */
/*   Updated: 2026/02/05 18:06:28 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <string>
#include <map>
#include <functional>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include "Packet.hpp"

class	Server
{
	public:
		static constexpr int	MAX_CLIENTS = 16;

		class	Client
		{
			public:
				Client(uint fd)
				{
					_fd = fd;
				}
				~Client() {}
			
				uint	fd() const {return (_fd);}
			private:
				uint		_fd;
		};
	public:
		Server() {}
		~Server() {}

		void	open(int port);
		void	close();
		void	update();

		void	setConnectCallback(std::function<void(const Server::Client &)> func) {_onConnect = func;}
		void	setDisconnectCallback(std::function<void(const Server::Client &)> func) {_onDisconnect = func;}
		void	setMessageCallback(std::function<void(const Server::Client &, Packet *)> func) {_onMessage = func;}

		/*
			Sends a message to all clients except the client given in parameters

			@throws runtime_error if send fails
		*/
		void	sendAll(const Server::Client &client, const std::string &msg) const
		{
			for (auto pair : _clients)
			{
				if (client.fd() != pair.second.fd())
					if (send(pair.second.fd(), msg.c_str(), msg.size(), 0) == -1)
						throw std::runtime_error("Failed to send message to fd " + std::to_string(pair.second.fd()));
			}
		}
		/*
			@throws runtime_error if send fails
		*/
		void	sendAll(const std::string &msg) const
		{
			for (auto pair : _clients)
				if (send(pair.second.fd(), msg.c_str(), msg.size(), 0) == -1)
					throw std::runtime_error("Failed to send message to fd " + std::to_string(pair.second.fd()));
		}
		/*
			@throws runtime_error if send fails
		*/
		void	sendClient(const Server::Client &client, const std::string &msg) const
		{
			if (send(client.fd(), msg.c_str(), msg.size(), 0) == -1)
				throw std::runtime_error("Failed to send message to fd " + std::to_string(client.fd()));
		}

		void	setPacketType(int id, std::function<Packet*(int)> func) {_packetTypes[id] = func;}
	private:
		void		_processInput(Server::Client &client, Packet &packet);
		void		_refreshPoll();
		void 		_recvClients();
		void		_addNewClient();
		std::map<int, Server::Client>	_clients;

		int						_socketFD = -1;
		sockaddr_in				_serverAddress;
		struct 	pollfd			_fds[Server::MAX_CLIENTS + 1];

		std::function<void(const Server::Client &)>							_onConnect = NULL;
		std::function<void(const Server::Client &)>							_onDisconnect = NULL;
		std::function<void(const Server::Client &, Packet *)>	_onMessage = NULL;
		
		std::map<int, std::function<Packet*(int)>>						_packetTypes;
};

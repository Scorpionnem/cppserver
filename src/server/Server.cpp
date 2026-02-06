#include "Server.hpp"

void	Server::update()
{
	_refreshPoll();
	int	pollEvents = poll(_fds, _clients.size() + 1, -1);
	if (pollEvents == -1 && errno == EINTR)
		return ;

	if ((_fds[0].revents & POLLIN) != 0)
	{
		_addNewClient();
		return ;
	}

	_recvClients();
}

/*
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
*/

void	Server::_processInput(Server::Client &client, Packet::Header &header)
{
	std::cout << "Received packet with Id: " << header.id << std::endl;

	auto find = _packetTypes.find(header.id);
	if (find == _packetTypes.end())
	{
		std::cerr << "Packet id not found" << std::endl;
		return ;
	}
	auto	funcs = find->second;
	auto	createFunc = funcs.second;
	auto	sizeFunc = funcs.first;

	Packet	*fullPacket = createFunc();

	ssize_t	payloadSize = sizeFunc() - sizeof(Packet::Header);
	ssize_t	size = recv(client.fd(), reinterpret_cast<char*>(fullPacket) + sizeof(Packet::Header), payloadSize, 0);

	if (size != payloadSize)
	{
		std::cerr << "Invalid packet payload received" << std::endl;
		delete fullPacket;
		return ;
	}

	fullPacket->hdr.id = header.id;

	if (_onMessage)
		_onMessage(client, fullPacket);

	delete fullPacket;
}

void Server::_recvClients()
{
	int i = 1;

	for (auto it = _clients.begin(); it != _clients.end();)
	{
		Server::Client	&client = it->second;

		if (_fds[i].revents & POLLIN)
		{
			Packet::Header	header;
			ssize_t 		size;

			size = recv(client.fd(), &header, sizeof(Packet::Header), 0);
			if (size == 0 || size == -1)
			{
				if (_onDisconnect)
					_onDisconnect(client);
				it = _clients.erase(it);
				goto skip_it;
			}
			if (size != sizeof(Packet::Header))
				std::cerr << "Invalid packet header size " << size << std::endl;
			else
				_processInput(client, header);
		}

		++it;
	skip_it:
		++i;
	}
}

void	Server::_addNewClient()
{
	sockaddr_in		addr;
	unsigned int	len = sizeof(sockaddr_in);
	char			ip[INET_ADDRSTRLEN];
	int				clientFD;

	std::memset(&addr, 0, sizeof(sockaddr_in));
	clientFD = accept(_socketFD, (struct sockaddr*)&addr, &len);
	if (clientFD == -1)
		return ;

	if (_clients.size() >= Server::MAX_CLIENTS)
	{
		::close(clientFD);
		return ;
	}

	inet_ntop(AF_INET, &addr, ip, INET_ADDRSTRLEN);

	Server::Client	client(clientFD);
	_clients.insert({clientFD, client});

	if (_onConnect)
		_onConnect(client);
}

void	Server::_refreshPoll()
{
	_fds[0].fd = _socketFD;
	_fds[0].events = POLLIN;
	_fds[0].revents = 0;
	int	i = 1;
	for (auto pair : _clients)
	{
		Server::Client	&client = pair.second;
		_fds[i].fd = client.fd();
		_fds[i].events = POLLIN;
		_fds[i].revents = 0;
		i++;
	}
}

void	Server::open(int port)
{
	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFD == -1)
		throw std::runtime_error(strerror(errno));
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(port);
	_serverAddress.sin_addr.s_addr = INADDR_ANY;

	int yes = 1;
	if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		::close(_socketFD);
		throw std::runtime_error(strerror(errno));
	}
	if (bind(this->_socketFD, (struct sockaddr*)&this->_serverAddress, sizeof(this->_serverAddress)) == -1)
	{
		::close(_socketFD);
		throw std::runtime_error(strerror(errno));
	}
	if (listen(_socketFD, MAX_CLIENTS) == -1)
	{
		::close(_socketFD);
		throw std::runtime_error(strerror(errno));
	}
	std::clog << "Server opened on " << port << std::endl;
}

void	Server::close()
{
	for (auto pair : _clients)
		::close(pair.first);
	if (_socketFD != -1)
		::close(_socketFD);
}

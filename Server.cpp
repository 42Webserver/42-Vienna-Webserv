#include "Server.hpp"

Server::Server(void) {}

Server::Server(const Server& a_other) : m_sockets(a_other.m_sockets) {}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		m_sockets = other.m_sockets;
		// m_requests = other.m_requests;
	}
	return (*this);
}

Server::~Server() {}

pollfd	newSock(int a_sockFd) //make to memeber
{
	pollfd newPfd;
	newPfd.fd = a_sockFd;
	newPfd.events = POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	return (newPfd);
}

int Server::initServerSocket(int a_ip, int a_port)
{
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_port = htons(a_port);
	m_serverAddress.sin_addr.s_addr = a_ip;

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		std::cerr << "Error: socket creation failed.\n";
		return (-1);
	}
	int enable = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
	{
		std::cerr << "Error: setsockopt failed.\n";
		return (-1);
	}
	if (bind(sock, (struct sockaddr *)&m_serverAddress, sizeof(m_serverAddress)) == -1)
	{
		std::cerr << "Error: binding socket to ip failed.\n";
		return (-1);
	}
	std::cout << m_serverAddress.sin_addr.s_addr << " " << m_serverAddress.sin_port << std::endl;
	if (listen(sock, 16) == -1)
	{
		std::cerr << "Error: listen failed.\n";
		return (-1);
	}
	m_sockets.push_back(newSock(sock));
	return (0);
}

pollfd	acceptNewConnection(int a_sockFd) //make to memeber
{
	struct sockaddr clientAdr;
	socklen_t		addrlen = sizeof(struct sockaddr);
	int clientFd = accept(a_sockFd, &clientAdr, &addrlen);
	if (clientFd == -1)
		std::cerr << "Error: fatal, accept fail.\n";
	std::cout << "new socket: " << clientFd << '\n';

	return (newSock(clientFd));
}

int Server::serverPoll(void)
{
	// std::cout << "Polling " << m_sockets.size() << " sockets, server socket: " << m_sockets.at(0).fd << std::endl;
	int	pollRet = poll(m_sockets.data(), m_sockets.size(), 100);
	if (pollRet == 0)
	{
		// std::cout << "Poll timeout. Lets move on\n";
		return (0);
	}
	else if (pollRet == -1)
		return (-1);
	std::cout << pollRet << " sockets triggered by events\n";
	for (std::size_t i = 0; i < m_sockets.size(); i++)
	{
		pollfd& curr = m_sockets.at(i);
		if (curr.revents & POLLIN)
		{
			std::cout << "Do read/accept request.\n";
			std::vector<Connection>::iterator conn = std::find(m_connections.begin(), m_connections.end(), curr.fd);

			if (conn == m_connections.end())
			{
				pollfd newSocket = acceptNewConnection(curr.fd);
				m_sockets.push_back(newSocket);
				m_connections.push_back(Connection(newSocket.fd));
			}
			else if (conn != m_connections.end())
			{
				if (conn->reciveRequestRaw() == -1)
					std::cerr << "err\n";
				conn->printHeadNBody();
			}

/* 			Connection clientConnection(head);
			clientConnect.init(); */

		}
		if (curr.revents & POLLOUT)
		{
			std::cout << "Pollout triggered" << '\n';
			std::vector<Connection>::iterator found = std::find(m_connections.begin(), m_connections.end(), curr.fd);
			if (found != m_connections.end())
			{
				std::cout << "Return response to request. On socket: " << curr.fd << '\n';
				send(found->getSocketFd(), "lul", 3, 0);
				close(found->getSocketFd());
				m_sockets.erase(m_sockets.begin() + i);
				m_connections.erase(found);
				std::cout << "Close socket " << curr.fd << '\n';
			}
		}
		if (curr.revents & POLLERR)
			std::cout << "Socket error.\n";
		if (curr.revents & POLLHUP)
			std::cout << "Socket hang-up.\n";
		if (curr.revents & POLLNVAL)
			std::cout << "Invalid request: socket not open.\n";
	}

	return (0);
}

const std::vector<pollfd>& Server::getSockets(void) const
{
	return (m_sockets);
}

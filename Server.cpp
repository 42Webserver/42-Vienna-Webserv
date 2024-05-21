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

pollfd	Server::newSocket(int a_sockFd)
{
	pollfd newPfd;
	newPfd.fd = a_sockFd;
	newPfd.events = POLLIN | POLLERR | POLLHUP | POLLNVAL;
	newPfd.revents = 0;
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
	m_sockets.push_back(newSocket(sock));
	m_sockets.front().events |= POLLIN;
	return (0);
}

int	Server::acceptNewConnection(int a_sockFd) //make to memeber
{
	struct sockaddr clientAdr;
	socklen_t		addrlen = sizeof(struct sockaddr);
	int clientFd = accept(a_sockFd, &clientAdr, &addrlen);
	if (clientFd == -1)
	{
		std::cerr << "Error: fatal, accept fail.\n";
		return (-1);
	}
	std::cout << "new socket: " << clientFd << '\n';
	pollfd socket = newSocket(clientFd);
	m_sockets.push_back(socket);
	m_connections.push_back(Connection(socket.fd));
	return (0);
}

int Server::serverPoll(void)
{
	// std::cout << "Polling " << m_sockets.size() << " sockets, server socket: " << m_sockets.at(0).fd << std::endl;
	int	pollRet = poll(m_sockets.data(), m_sockets.size(), 10);
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
		std::cout << "Socket " << m_sockets.at(i).fd << "	 events: " << m_sockets.at(i).revents << '\n';
		if (m_sockets.at(i).revents & POLLIN)
		{
			std::cout << "Do read/accept request.\n";
			std::vector<Connection>::iterator conn = std::find(m_connections.begin(), m_connections.end(), m_sockets.at(i).fd);

			if (conn == m_connections.end())
			{
				if (acceptNewConnection(m_sockets.at(i).fd) == -1)
					std::cerr << "Error: accepting new client connection\n";
				
			}
			else if (conn != m_connections.end())
			{
				if (conn->reciveRequestRaw() == -1)
					std::cerr << "Error: recv\n";
				m_sockets.at(i).events |= POLLOUT;
				conn->printHeadNBody();
			}

			/* Connection clientConnection(head);
			clientConnect.init(); */

		}
		if (m_sockets.at(i).revents & POLLOUT)
		{
			std::cout << "Pollout triggered" << '\n';
			std::vector<Connection>::iterator found = std::find(m_connections.begin(), m_connections.end(), m_sockets.at(i).fd);
			if (found != m_connections.end())
			{
				std::cout << "Return response to request. On socket: " << m_sockets.at(i).fd << '\n';
				if (found->sendResponse() == -1)
					std::cerr << "Error: sending" << '\n';
				close(found->getSocketFd());
				// std::cout << "Close socket " << m_sockets.at(i).fd << '\n';
				m_sockets.erase(m_sockets.begin() + i);
				m_connections.erase(found);
				i--;
				continue ;
			}
		}
		if (m_sockets.at(i).revents & POLLERR)
			std::cout << "Socket error.\n";
		if (m_sockets.at(i).revents & POLLHUP)
		{
			std::cout << "Socket hang-up.\n";
			exit(1);
		}
		if (m_sockets.at(i).revents & POLLNVAL)
			std::cout << "Invalid request: socket not open.\n";
	}

	return (pollRet);
}

const std::vector<pollfd>& Server::getSockets(void) const
{
	return (m_sockets);
}

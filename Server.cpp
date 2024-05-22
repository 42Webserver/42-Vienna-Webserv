#include "Server.hpp"

Server::Server(void) {}

Server::Server(const Server& a_other) : m_serverSocket(a_other.m_serverSocket), m_serverAddress(a_other.m_serverAddress) {}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		m_serverSocket = other.m_serverSocket;
		m_serverAddress = other.m_serverAddress;
	}
	return (*this);
}

Server::~Server() {}

int Server::getServerSocket(void) const
{
	return (m_serverSocket);
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
	m_serverSocket = sock;
	return (sock);
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
	return (clientFd);
}

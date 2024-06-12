#include "Server.hpp"

Server::Server(void) {}

Server::Server(const Server& a_other) : m_serverSocket(a_other.m_serverSocket), m_serverAddress(a_other.m_serverAddress), m_subServers(a_other.m_subServers) {}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		m_serverSocket = other.m_serverSocket;
		m_serverAddress = other.m_serverAddress;
		m_subServers = other.m_subServers;
	}
	return (*this);
}

Server::~Server() {}

int Server::getServerSocket(void) const
{
	return (m_serverSocket);
}

void Server::addSubServer(const subserver &a_subServer)
{
	m_subServers.push_back(a_subServer);
}

short Server::getPort(void) const
{
	if (m_subServers.size() == 0)
		return (80);
	return (m_subServers.at(0).getPort());
}

int Server::initServerSocket()
{
	if (m_subServers.size() == 0) {
		throw (std::runtime_error("Error: server: No server config."));
	}
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_port = htons(m_subServers.at(0).getPort());
	m_serverAddress.sin_addr.s_addr = htonl(m_subServers.at(0).getIp());

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		throw (std::runtime_error("Error: server: socket creation failed."));
	}
	int enable = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
	{
		throw (std::runtime_error("Error: server: setsockopt failed."));
	}
	if (bind(sock, (struct sockaddr *)&m_serverAddress, sizeof(m_serverAddress)) == -1)
	{
		throw (std::runtime_error("Error: server: binding socket to ip failed."));
	}
	// std::cout << m_serverAddress.sin_addr.s_addr << " " << m_serverAddress.sin_port << std::endl;
	if (listen(sock, 16) == -1)
	{
		throw (std::runtime_error("Error: server: setting socket to listen failed."));
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
	// std::cout << "new socket: " << clientFd << '\n';
	return (clientFd);
}

bool Server::operator==(int a_port) const
{
	return (this->getPort() == a_port);
}

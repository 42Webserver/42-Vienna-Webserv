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

u_int16_t Server::getPort(void) const
{
	if (m_subServers.size() == 0)
		return (80);
	return (m_subServers.at(0).getPort());
}

u_int64_t Server::getIp(void) const
{
	if (m_subServers.size() == 0)
		return (0);
	return (m_subServers.at(0).getIp());
}

std::string Server::getHost(void) const
{
	if (m_subServers.size() == 0)
		return (0);
    return (m_subServers.at(0).getHost());
}

subserver& Server::getSubServer(const std::string &a_hostname)
{
	subserver& defaultSubServer = m_subServers.at(0);
	for (size_t i = 0; i < m_subServers.size(); i++)
	{
		if (m_subServers.at(i).serverConfig.find("server_name") != m_subServers.at(i).serverConfig.end())
		{
			for (size_t j = 0; j < m_subServers.at(i).serverConfig.at("server_name").size(); j++)
			{
				if (m_subServers.at(i).serverConfig.at("server_name").at(j) == "_")
					defaultSubServer = m_subServers.at(i);
				if (m_subServers.at(i).serverConfig.at("server_name").at(j) == a_hostname)
					return (m_subServers.at(i));
			}
		}
	}
    return (defaultSubServer);
}

void Server::setServerAddress()
{
	int error_code;
	struct addrinfo input, *result; 
	
	memset(&input, 0, sizeof(input));
	input.ai_family = AF_INET; //IPV4;
	input.ai_socktype = SOCK_STREAM; //TCP connection; 

	std::cout << "HOST = " << m_subServers.at(0).getHost().c_str() << std::endl;
	if ((error_code = getaddrinfo(m_subServers.at(0).getHost().c_str(), "http", &input, &result)) != 0)
		throw (std::runtime_error("Error: server: Host not found"));
	
	m_serverAddress = (struct sockaddr_in *)result->ai_addr;
	m_serverAddress->sin_port = htons(m_subServers.at(0).getPort());
	std::cout << "Host address = " << m_serverAddress->sin_addr.s_addr << '\n';
}


int Server::initServerSocket()
{
	if (m_subServers.size() == 0) {
		throw (std::runtime_error("Error: server: No server config."));
	}
	setServerAddress();
	/* m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_port = htons(m_subServers.at(0).getPort());
	m_serverAddress.sin_addr.s_addr = htonl(m_subServers.at(0).getIp()); */

	int sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (sock == -1)
	{
		throw (std::runtime_error("Error: server: socket creation failed."));
	}
	int enable = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
	{
		throw (std::runtime_error("Error: server: setsockopt failed."));
	}
	if (bind(sock, (struct sockaddr *)m_serverAddress, sizeof(*m_serverAddress)) == -1)
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

/// @brief Accept a new client connection on the serverSocket.
/// @param void
/// @return Returns client socket file descriptor.
int	Server::acceptNewConnection(void)
{
	struct sockaddr clientAdr;
	socklen_t		addrlen = sizeof(struct sockaddr);
	int clientFd = accept(m_serverSocket, &clientAdr, &addrlen);
	if (clientFd == -1)
	{
		std::cerr << "Error: fatal, accept fail.\n";
		return (-1);
	}
	// std::cout << "new socket: " << clientFd << '\n';
	return (clientFd);
}

bool Server::operator==(struct serveradress a_listen) const
{
	return (this->getPort() == a_listen.getPort() && this->getIp() == a_listen.getIp());
}

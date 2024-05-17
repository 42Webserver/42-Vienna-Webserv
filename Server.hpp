#ifndef SERVER_H
# define SERVER_H

#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include "Connection.hpp"
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <algorithm>

class Server
{
private:
	std::vector<pollfd>		m_sockets;
	std::vector<Connection>	m_connections;
	sockaddr_in				m_serverAddress;

public:
	Server(void);
	// Server(long a_ip, long a_port);
	Server(const Server& other);
	Server& operator=(const Server& other);
	~Server();

	int	initServerSocket(int a_ip, int a_port);
	int	serverPoll(void);
	const std::vector<pollfd>& getSockets(void) const;
};

#endif // !SERVER_H

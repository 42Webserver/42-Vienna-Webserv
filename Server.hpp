#ifndef SERVER_HPP
# define SERVER_HPP

#include <map>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include "Connection.hpp"
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <algorithm>
#include "Data.hpp"

class Server
{
private:

	int									m_serverSocket;
	sockaddr_in							m_serverAddress;
	std::vector<struct subserver>		m_subServers;

public:
	Server(void);
	Server(const Server& other);
	Server& operator=(const Server& other);
	~Server();

	int	getServerSocket(void) const;

	void	addSubServer(const struct subserver& a_subServer);
	short	getPort(void) const;

	int	initServerSocket();

	int	acceptNewConnection(int a_sockFd);

	bool	operator==(int a_port) const;
};

#endif // !SERVER_HPP

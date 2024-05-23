#ifndef SERVER_HPP
# define SERVER_HPP

#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
// #include "Connection.hpp"
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <algorithm>
#include <map>

class Server
{
private:

	int									m_serverSocket;
	sockaddr_in							m_serverAddress;
	std::map<std::string, std::string> 	m_config;				//for testing!

public:
	Server(void);
	Server(const Server& other);
	Server& operator=(const Server& other);
	~Server();

	int	getServerSocket(void) const;

	int	initServerSocket(int a_ip, int a_port);

	int	acceptNewConnection(int a_sockFd);
};

#endif // !SERVER_HPP

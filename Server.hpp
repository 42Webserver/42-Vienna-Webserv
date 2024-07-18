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
#include <netdb.h>

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

	void		addSubServer(const struct subserver& a_subServer);
	u_int16_t	getPort(void) const;
	u_int64_t	getIp(void) const;
	std::string	getHost(void) const;
	void	setServerAddress();

	struct subserver&	getSubServer(const std::string& a_hostname);

	int	initServerSocket();
	int	acceptNewConnection();

	bool	operator==(struct serveradress a_listen) const;
};

#endif // !SERVER_HPP

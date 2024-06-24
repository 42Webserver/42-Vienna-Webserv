#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Debug.h"

#define BUFFER_SIZE 80000

class Server;

class Connection
{
private:

	Server&		m_server;
	int			m_clientSocket;

	time_t		m_idleStart;
	Response	m_response;
	Request		m_request;

	int		readAppend(std::string& a_appendString);
	int		readAppend(std::string& a_appendString, std::size_t a_size);
	int		readHead(void);
	int		readBody(void);

	Connection(void);

public:

	Connection(Server& a_server, int a_clientSocket);
	Connection(const Connection& a_other);
	~Connection();

	Connection&	operator=(const Connection& a_other);
	bool		operator==(const int a_fd) const;

	int	getSocketFd(void) const;
	int	receiveRequestRaw(void);
	int	sendResponse(void);

	time_t	getIdleTime(void) const;
};

#endif // !CONNECTION_HPP

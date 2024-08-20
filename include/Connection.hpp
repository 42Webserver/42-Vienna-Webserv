#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Debug.hpp"
#include "Timer.hpp"

#define BUFFER_SIZE 80000

class Server;

class Connection
{
private:

	Server&		m_server;
	int			m_clientSocket;
	int			m_cgiFd;

	Request		m_request;
	Response	m_response;

	int			readAppend(std::string& a_appendString);
	int			readHead(void);
	int			readBody(void);

	Connection(void);

public:

	Timer		idleTime;
	
	Connection(Server& a_server, int a_clientSocket);
	Connection(const Connection& a_other);
	~Connection();

	Connection&	operator=(const Connection& a_other);
	bool		operator==(const int a_fd) const;

	int		getSocketFd(void) const;
	int		receiveRequestRaw(void);
	int		sendResponse(void);
	void	closeConnection(void);
	bool	isResponseCgi(void) const;
};

#endif // !CONNECTION_HPP

#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

#define BUFFER_SIZE 16

class Server;

class Connection
{
private:

	Server&		m_server;
	int			m_clientSocket;

	std::string	m_head;
	std::string	m_body;

	time_t		m_idleStart;

	Response	m_response;
	Request		m_request;

	std::string	readUntilSep(std::string& a_ouput, const std::string& a_seperator);
	Connection(void);

public:

	Connection(Server& a_server, int a_clientSocket);
	Connection(const Connection& a_other);
	~Connection();

	Connection&	operator=(const Connection& a_other);
	bool		operator==(const int a_fd) const;

	int	getSocketFd(void) const;
	int	reciveRequestRaw(void);
	int	sendResponse(void);

	time_t	getIdleTime(void) const;


	//debug
	void printHeadNBody(void) const;
};

#endif // !CONNECTION_HPP

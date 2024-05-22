#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "Request.hpp"
#include "Response.hpp"

#define BUFFER_SIZE 16

class Connection
{
private:

	int			m_clientSocket;

	std::string	m_head;
	std::string	m_body;

	Response	m_response;
	Request		m_request;

	std::string	readUntilSep(std::string& a_ouput, const std::string& a_seperator);
	Connection(void);

public:

	Connection(int a_clientSocket);
	Connection(const Connection& a_other);
	~Connection();

	Connection&	operator=(const Connection& a_other);
	bool		operator==(const int a_fd) const;

	int	getSocketFd(void) const;
	int	reciveRequestRaw(void);
	int	sendResponse(void) const;


	//debug
	void printHeadNBody(void) const;
};

#endif // !CONNECTION_HPP

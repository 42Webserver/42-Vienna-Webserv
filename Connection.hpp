#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

#define BUFFER_SIZE 1024

#define TERMC_RED "\033[91m"
#define TERMC_GREEN "\033[92m"
#define TERMC_DARKGREEN "\033[32m"
#define TERMC_BLUE "\033[94m"
#define TERMC_ORANGE "\033[33m"
#define TERMC_CYAN "\033[36m"

#ifdef DEBUG
#define LOG(msg) std::cout << msg << '\n';
#define LOGC(colour, msg) std::cout << colour << msg << "\033[39m" << '\n';
#else
#define LOG(msg)
#define LOGC(colour, msg)
#endif

class Server;

class Connection
{
private:

	Server&		m_server;
	int			m_clientSocket;

	std::string	m_head;
	std::string	m_body;

	time_t		m_idleStart;
	bool		m_chunked;
	Response	m_response;
	Request		m_request;

	int		readAppend(std::string& a_appendString);
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


	//debug
	void printHeadNBody(void) const;
};

#endif // !CONNECTION_HPP

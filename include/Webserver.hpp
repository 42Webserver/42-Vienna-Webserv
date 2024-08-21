#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <map>
#include <vector>
#include <iostream>
#include <poll.h>
#include <fstream>
#include <map>
#include <csignal>
#include "Server.hpp"
#include "PollContainer.hpp"
#include "Data.hpp"
#include "ConfigParser.hpp"

class Webserver
{
private:

	std::vector<Server>		m_servers;
	PollContainer			m_polls;

	int		pollServers(void);
	int		pollClients(void);

	void	removeTimeoutConnections(void);

public:

	Webserver(void);
	Webserver(const Webserver& other);
	Webserver& operator=(const Webserver& other);
	~Webserver();

	int		init(std::vector<struct subserver> subservers);
	int		runServer(void);
};


#endif // !WEBSERVER_HPP

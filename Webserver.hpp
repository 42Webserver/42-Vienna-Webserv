#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <map>
#include <vector>
#include "Server.hpp"
#include <poll.h>
#include "PollContainer.hpp"
#include <fstream>
#include "Data.hpp"
#include "ConfigParser.hpp"
#include <map>

class Webserver
{
private:

	std::vector<Server>		m_servers;
	PollContainer			m_polls;

	int	pollServers(void);
	int	pollClients(void);

public:

	Webserver(void);
	Webserver(std::vector<struct subserver> subservers);
	Webserver(const Webserver& other);
	Webserver& operator=(const Webserver& other);
	~Webserver();

	int	run();
};


#endif // !WEBSERVER_HPP

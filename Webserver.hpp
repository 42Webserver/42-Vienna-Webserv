#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <vector>
#include "Server.hpp"
#include <poll.h>
#include "PollContainer.hpp"

class Webserver
{
private:

	std::vector<Server>		m_servers;
	PollContainer			m_polls;

public:

	Webserver(void);
	Webserver(const Webserver& other);
	Webserver& operator=(const Webserver& other);
	~Webserver();

	int	pollServers(void);
	int pollClients(void);
	int	run();
};


#endif // !WEBSERVER_HPP

#ifndef WEBSERVER_H
# define WEBSERVER_H

#include <vector>
#include "Server.hpp"
#include <poll.h>

class Webserver
{
private:

	std::vector<Server>	m_servers;

public:

	Webserver(void);
	Webserver(const Webserver& other);
	Webserver& operator=(const Webserver& other);
	~Webserver();

	std::vector<pollfd>	serversToPollfd(const std::vector<Server>& a_servers) const;
	int	run();
};


#endif // !WEBSERVER_H

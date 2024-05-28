#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <map>
#include <vector>
#include "Server.hpp"
#include <poll.h>
#include "PollContainer.hpp"
#include <fstream>

class Webserver
{
private:


	std::vector<Server>	m_servers;
	PollContainer		m_polls;

public:

	Webserver(void);
	Webserver(const std::string& configName);
	Webserver(const Webserver& other);
	Webserver& operator=(const Webserver& other);
	~Webserver();

	int	pollServers(void);
	int	pollClients(void);

	void	replaceWhitespaces(std::string& str);
	void	separateSpecialChars(std::string& str);
	void    removeComments(std::string& str);
	void	processLine(std::string& line);
	void	readConfigFile(const std::string& file);

	int	run();
};


#endif // !WEBSERVER_HPP

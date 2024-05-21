#include "Webserver.hpp"

Webserver::Webserver(void)
{
	Server testSrvr;
	if (testSrvr.initServerSocket(INADDR_ANY, 8080) != -1)
		m_servers.push_back(testSrvr);
	if (testSrvr.initServerSocket(INADDR_ANY, 8090) != -1)
		m_servers.push_back(testSrvr);
	else
		std::cerr << "fail port 8090" << '\n';
}

Webserver::Webserver(const Webserver& a_other) : m_servers(a_other.m_servers) {}

Webserver&	Webserver::operator=(const Webserver& a_other)
{
	if (this != &a_other)
	{
		m_servers = a_other.m_servers;
	}
	return (*this);
}

Webserver::~Webserver() {}

int	Webserver::run()
{
	int	pollRet	= -1;
	do
	{
		for (std::size_t i = 0; i < m_servers.size(); i++)
		{
			pollRet = m_servers.at(i).serverPoll();
			if (pollRet == -1)
			{
				std::cerr << "Poll error fatal wow oh no!\n";
				return (-1);
			}
		}
	} while (pollRet != -1);


	// std::vector<pollfd>	pollfds = serversToPollfd(m_servers);

	// int	pollRet = 0;
	// while (pollRet != -1)
	// {
	// 	pollRet = poll(pollfds.data(), pollfds.size(), -1);
	// 	if (pollRet == -1)
	// 		std::cerr << "Error: poll error.\n";
	// 	else if (pollRet == 0)
	// 		std::cout << "Poll timeout\n";
	// 	else
	// 	{
	// 		for (std::size_t i = 0; i < pollfds.size(); i++)
	// 		{
	// 			pollfd&	curr = pollfds.at(i);
	// 			if (curr.revents & POLLIN)
	// 			{
	// 				std::cout << "incoming connection" << std::endl;
	// 			}
	// 			else if (curr.revents & POLLOUT)
	// 				std::cout << "ready for outgoing connection" << std::endl;
	// 			else
	// 				std::cout << "No poll event" << std::endl;
	// 		}
	// 	}
	// }
	return (0);
}

std::vector<pollfd> Webserver::serversToPollfd(const std::vector<Server> &a_servers) const
{
	std::vector<pollfd>	pollFds;
	for (std::size_t i = 0; i < a_servers.size(); i++)
	{
		const std::vector<pollfd>& sockets = a_servers.at(i).getSockets();
		pollFds.insert(pollFds.end(), sockets.begin(), sockets.end());

	}

	return (pollFds);
}

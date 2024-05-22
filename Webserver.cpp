#include "Webserver.hpp"

Webserver::Webserver(void)
{
	Server testSrvr;
	if (testSrvr.initServerSocket(INADDR_ANY, 8080) != -1)
	{
		m_servers.push_back(testSrvr);
		m_polls.addServer(testSrvr);
	}
	// if (testSrvr.initServerSocket(INADDR_ANY, 8090) != -1)
	// {
	// 	m_servers.push_back(testSrvr);
	// 	m_polls.addServer(testSrvr);
	// }
	// else
	// 	std::cerr << "fail port 8090" << '\n';
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

int Webserver::pollServers(void)
{
	for (std::size_t i = 0; i < m_servers.size(); i++)
	{
		if (m_polls.getPollfdsAt(i).revents & POLLIN)
		{
			int client_socket = m_servers.at(i).acceptNewConnection();
			if (client_socket == -1)
			{
				std::cerr << "Server Error: could not accept new client" << '\n';
				continue ;
			}
			m_polls.addConnection(Connection(m_servers.at(i), client_socket));
		}
 		if (m_polls.getPollfdsAt(i).revents & POLLOUT)
			std::cout << "Pollout triggered" << '\n';
		if (m_polls.getPollfdsAt(i).revents & POLLERR)
			std::cout << "Socket error.\n";
		if (m_polls.getPollfdsAt(i).revents & POLLHUP)
			std::cout << "Socket hang-up.\n";
		if (m_polls.getPollfdsAt(i).revents & POLLNVAL)
			std::cout << "Invalid request: socket not open.\n";
	}
	return (0);
}

std::ostream& operator<<(std::ostream& os, const pollfd& a_pollfd)
{
	os << "Fd: " << a_pollfd.fd << '\n';
	os << "events: " << (a_pollfd.events & POLLIN ? "POLLIN " : "")
		<< (a_pollfd.events & POLLOUT ? "POLLOUT " : "")
		<< (a_pollfd.events & POLLHUP ? "POLLHUP " : "")
		<< (a_pollfd.events & POLLERR ? "POLLERR " : "")
		<< (a_pollfd.events & POLLNVAL ? "POLLNVAL " : "") << '\n';
	os << "revents: " << (a_pollfd.revents & POLLIN ? "POLLIN " : "")
		<< (a_pollfd.revents & POLLOUT ? "POLLOUT " : "")
		<< (a_pollfd.revents & POLLHUP ? "POLLHUP " : "")
		<< (a_pollfd.revents & POLLERR ? "POLLERR " : "")
		<< (a_pollfd.revents & POLLNVAL ? "POLLNVAL " : "") << '\n';
	return (os);
}

int Webserver::pollClients(void)
{
	for (std::size_t i = m_servers.size(); i < m_polls.getPollfds().size(); i++)
	{
		std::cout << "Socket " << m_polls.getPollfdsAt(i).fd << "	 events: " << m_polls.getPollfdsAt(i).revents << '\n';
		if (m_polls.getPollfdsAt(i).revents & POLLIN)
		{
			std::cout << "Do read/accept request.\n";
			std::cout << m_polls.getPollfdsAt(i);
			if (m_polls.getConnection(i).reciveRequestRaw() == -1)
				std::cerr << "Mal schaun\n";
			m_polls.getPollfdsAt(i).events |= POLLOUT;
			m_polls.getPollfdsAt(i).events ^= POLLIN;
		}
		if (m_polls.getPollfdsAt(i).revents & POLLOUT)
		{
			std::cout << "Pollout triggered\n";
			if (m_polls.getConnection(i).sendResponse() == -1)
				std::cerr << "Error: send\n";
			// close(m_polls.getPollfdsAt(i).fd);
			// m_polls.removeConnection(i--); 	//? Sicher
			// continue ;						//?
			m_polls.getPollfdsAt(i).events |= POLLIN;
			m_polls.getPollfdsAt(i).events ^= POLLOUT;
		}
		if (m_polls.getPollfdsAt(i).revents & POLLHUP)
		{
			std::cout << "Socket hang-up.\n";
			m_polls.removeConnection(i--);
			continue;
		}
		if (m_polls.getPollfdsAt(i).revents & POLLERR)
		{
			std::cerr << "Socket error: \n" << m_polls.getPollfdsAt(i);
			m_polls.removeConnection(i--);
			continue;
		}
		if (m_polls.getPollfdsAt(i).revents & POLLNVAL)
		{
			std::cerr << "Invalid request: socket not open.\n";
			m_polls.removeConnection(i--);
			continue;
		}
		m_polls.getPollfdsAt(i).revents = 0;
	}
	return (0);
}

int	Webserver::runServer()
{
	int	pollRet = 0;
	while (pollRet != -1)
	{
		pollRet = poll(m_polls.getPollfds().data(), m_polls.getPollfds().size(), -1);
		if (pollRet == -1)
			std::cerr << "Error: poll error.\n";
		else if (pollRet == 0)
			std::cout << "Poll timeout\n";
		else
		{
			if (pollServers() == -1)
			{
				std::cerr << "Poll server error\n";
				continue; //try save by clean function?
			}
			if (pollClients() == -1)
			{
				std::cerr << "Poll client error\n";
				continue; //? überlegen ob das sinn macht. Error handling geschichte
			}
		}
	}
	return (pollRet);
}

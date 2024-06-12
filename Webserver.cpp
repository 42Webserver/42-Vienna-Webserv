#include "Webserver.hpp"

Webserver::Webserver(void)
{
	// if (testSrvr.initServerSocket(INADDR_ANY, 8090) != -1)
	// {
	// 	m_servers.push_back(testSrvr);
	// 	m_polls.addServer(testSrvr);
	// }
	// else
	// 	std::cerr << "fail port 8090" << '\n';
}

Webserver::Webserver(std::vector<struct subserver> subservers)
{
	for (std::size_t i = 0; i < subservers.size(); i++)
	{
		std::cout << (m_servers.size() != 0 ? m_servers.at(0).getPort() : -1) << " | " << subservers.at(i).getPort() << '\n';
		std::vector<Server>::iterator found = std::find(m_servers.begin(), m_servers.end(), subservers.at(i).getPort());
		if (found == m_servers.end()) {
			Server	temp;
			temp.addSubServer(subservers.at(i));
			m_servers.push_back(temp);
			std::cout << "Add new server with subserver port:" << temp.getPort() << "\n";
		} else {
			found->addSubServer(subservers.at(i));
			std::cout << "Add new subserver\n";
		}
		// m_servers[subservers.at(i).getPort()].addSubServer(subservers.at(i));
		// std::cout << "Adding subserver: " << subservers.at(i).getPort() << '\n';
	}
	for (std::vector<Server>::iterator it = m_servers.begin(); it != m_servers.end(); it++)
	{
		it->initServerSocket();
		m_polls.addServer(*it);
		std::cout << "Init server, and add to Pollcontainer\n";
	}
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
			int client_socket = m_servers.at(i).acceptNewConnection(m_polls.getPollfdsAt(i).fd);
			if (client_socket == -1)
			{
				std::cerr << "Server Error: could not accept new client" << '\n';
				continue ;
			}
			m_polls.addConnection(Connection(m_servers.at(i), client_socket));
		}
/* 		if (m_polls.getPollfdsAt(i).revents & POLLOUT)
			std::cout << "Pollout triggered" << '\n'; */
		if (m_polls.getPollfdsAt(i).revents & POLLERR)
			std::cout << "Socket error.\n";
		if (m_polls.getPollfdsAt(i).revents & POLLHUP)
			std::cout << "Socket hang-up.\n";
		if (m_polls.getPollfdsAt(i).revents & POLLNVAL)
			std::cout << "Invalid request: socket not open.\n";
	}
	return (0);
}

int Webserver::pollClients(void)
{
	for (std::size_t i = m_servers.size(); i < m_polls.getPollfds().size(); i++)
	{
		std::cout << "Socket " << m_polls.getPollfdsAt(i).fd << "	 events: " << m_polls.getPollfdsAt(i).revents << '\n';
		if (m_polls.getPollfdsAt(i).revents & POLLIN)
		{
			std::cout << "Do read/accept request.\n";
			if (m_polls.getConnection(i).receiveRequestRaw() == -1)
				std::cerr << "Mal schaun\n";
			m_polls.getPollfdsAt(i).events |= POLLOUT;
		}
		if (m_polls.getPollfdsAt(i).revents & POLLOUT)
		{
			std::cout << "Pollout triggered\n";
			if (m_polls.getConnection(i).sendResponse() == -1)
				std::cerr << "Error: send\n";
			m_polls.removeConnection(i--); 	//? Sicher
			continue ;						//?
		}
		if (m_polls.getPollfdsAt(i).revents & POLLERR)
			std::cout << "Socket error.\n";
		if (m_polls.getPollfdsAt(i).revents & POLLHUP)
			std::cout << "Socket hang-up.\n";
		if (m_polls.getPollfdsAt(i).revents & POLLNVAL)
			std::cout << "Invalid request: socket not open.\n";
	}
	return (0);
}

int	Webserver::run()
{
	int	pollRet = 0;
	if (m_servers.size() == 0)
	{
		std::cerr << "NO servers\n";
		return (1);
	}
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
				continue; //try save by clean function?
			if (pollClients() == -1)
				continue; //? überlegen ob das sinn macht. Error handling geschichte
		}
	}
	return (0);
}

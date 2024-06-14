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
		//std::cout << (m_servers.size() != 0 ? m_servers.at(0).getPort() : -1) << " | " << subservers.at(i).getPort() << '\n';
		std::vector<Server>::iterator found = std::find(m_servers.begin(), m_servers.end(), subservers.at(i).getAdress());
		if (found == m_servers.end()) { //servers can run on different ips but same port.
			Server	temp;
			temp.addSubServer(subservers.at(i));
			m_servers.push_back(temp);
			//std::cout << "Add new server with subserver port:" << temp.getPort() << "\n";
		} else {
			found->addSubServer(subservers.at(i));
			//std::cout << "Add new subserver\n";
		}
		// m_servers[subservers.at(i).getPort()].addSubServer(subservers.at(i));
		// std::cout << "Adding subserver: " << subservers.at(i).getPort() << '\n';
	}
	for (std::vector<Server>::iterator it = m_servers.begin(); it != m_servers.end(); it++)
	{
		it->initServerSocket();
		m_polls.addServer(*it);
		//std::cout << "Init server, and add to Pollcontainer\n";
	}
}

Webserver::Webserver(const Webserver& a_other) : m_servers(a_other.m_servers), m_polls(a_other.m_polls) {}

Webserver&	Webserver::operator=(const Webserver& a_other)
{
	if (this != &a_other)
	{
		m_servers = a_other.m_servers;
		m_polls = a_other.m_polls;
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
		std::cout << m_polls.getPollfdsAt(i);
		if (m_polls.getPollfdsAt(i).revents & POLLHUP)
		{
			std::cout << "\033[94mSocket hang-up.\n\033[39m";
			m_polls.removeConnection(i--);
			continue;
		}
		if (m_polls.getPollfdsAt(i).revents & (POLLERR | POLLNVAL))
		{
			std::cerr << "\033[91mSocket error: \n" << m_polls.getPollfdsAt(i) << "\033[39m";
			m_polls.removeConnection(i--);
			continue;
		}
		if (m_polls.getPollfdsAt(i).revents & POLLIN)
		{
			std::cout << "\033[92mDo read/accept request.\n\033[39m";
			int ret = m_polls.getConnection(i).receiveRequestRaw();
			if (ret == 1){
				std::cout << "\033[91m" << "recv == 0, close connection?\n\033[39m";
				m_polls.removeConnection(i);
				continue;
			} if (ret == -1) {
				std::cerr << "Mal schaun\n";
				exit(42);
			}
			m_polls.getPollfdsAt(i).events ^= POLLOUT;
			m_polls.getPollfdsAt(i).events ^= POLLIN;
		}
		if (m_polls.getPollfdsAt(i).revents & POLLOUT)
		{
			std::cout << "\033[92mPollout triggered\n\033[39m";
			if (m_polls.getConnection(i).sendResponse() == -1)
				std::cerr << "Error: send\n";
			// close(m_polls.getPollfdsAt(i).fd);
			// m_polls.removeConnection(i--); 	//? Sicher
			// continue ;						//?
			m_polls.getPollfdsAt(i).events ^= POLLIN;
			m_polls.getPollfdsAt(i).events ^= POLLOUT;
		}
		if (m_polls.getConnection(i).getIdleTime() > 1) {
			std::cout << "\033[32m>>> Client time out <<<\n\033[39m";
			m_polls.removeConnection(i);
			continue;
		}
		m_polls.getPollfdsAt(i).revents = 0;
	}
	return (0);
}

int	Webserver::runServer()
{
	int	pollRet = 0;
	if (m_servers.size() == 0)
	{
		std::cerr << "NO servers\n";
		return (1);
	}
	while (pollRet != -1)
	{
		pollRet = poll(m_polls.getPollfds().data(), m_polls.getPollfds().size(), 100);
		if (pollRet == -1)
			std::cerr << "Error: poll error.\n";
		else if (pollRet == 0) {
			std::cout << "\033[96m" << "Poll timeout\n\033[39m";
			for (std::size_t i = m_polls.getPollfds().size() - 1; i >= m_servers.size(); i--)
			{
				if (m_polls.getConnection(i).getIdleTime() > 1) {
					std::cout << "\033[32m>>> Client time out <<<\n\033[39m";
					m_polls.removeConnection(i);
					continue;
				}
			}
		}
		else
		{
			std::cout << "\033[33m" << pollRet << " Poll(s) triggered\n" << "\033[39m";
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

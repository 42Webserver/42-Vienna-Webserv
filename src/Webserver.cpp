#include "Webserver.hpp"

bool	g_isRunning = true;

static void	stopServer(int a_sig)
{
	if (a_sig)
		g_isRunning = false;
}

Webserver::Webserver(void) {}

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

Webserver::~Webserver() 
{
	for (size_t i = 0; i < m_polls.getPollfds().size(); i++)
	{
		if (m_servers.size() > i)
			close(m_polls.getPollfdsAt(i).fd);
		else
			m_polls.getConnection(i).closeConnection();
	}
}

int Webserver::init(std::vector<struct subserver> subservers)
{
	signal(SIGINT, stopServer);
	for (size_t i = 0; i < subservers.size(); i++)
		freeaddrinfo(subservers.at(i).setServerAdress());
	for (std::size_t i = 0; i < subservers.size(); i++)
	{
		std::vector<Server>::iterator found = std::find(m_servers.begin(), m_servers.end(), subservers.at(i).getAdress());
		if (found == m_servers.end()) {
			Server	temp;
			temp.addSubServer(subservers.at(i));
			m_servers.push_back(temp);
		} else {
			found->addSubServer(subservers.at(i));
		}
	}
	for (std::vector<Server>::iterator it = m_servers.begin(); it != m_servers.end(); it++)
	{
		it->initServerSocket();
		m_polls.addServer(*it);
	}
	return (0);
}

int	Webserver::pollServers(void)
{
	for (std::size_t i = 0; i < m_servers.size(); i++)
	{
		if (m_polls.getPollfdsAt(i).revents & POLLIN)
		{
			int client_socket = m_servers.at(i).acceptNewConnection();
			if (client_socket == -1)
			{
				std::cout << "Server Error: could not accept new client" << '\n';
				continue ;
			}
			m_polls.addConnection(Connection(m_servers.at(i), client_socket));
		}
		if (m_polls.getPollfdsAt(i).revents & POLLERR)
			std::cout << "Server error: error on server socket\n";
		if (m_polls.getPollfdsAt(i).revents & POLLNVAL)
			std::cout << "Server error: bad socket.\n";
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

int	Webserver::pollClients(void)
{
	for (std::size_t i = m_servers.size(); i < m_polls.getPollfds().size(); i++)
	{
		LOG_INFO(m_polls.getPollfdsAt(i));
		if (m_polls.getPollfdsAt(i).revents & POLLHUP)
		{
			LOG_WARNING("Socket hang-up.")
			m_polls.removeConnection(i--);
			continue;
		}
		if (m_polls.getPollfdsAt(i).revents & (POLLERR | POLLNVAL))
		{
			LOG_ERROR("Socket error: \n" << m_polls.getPollfdsAt(i))
			m_polls.removeConnection(i--);
			continue;
		}
		if (m_polls.getPollfdsAt(i).revents & POLLIN)
		{
			LOG_INFO("Do read/accept request.")
			int ret = m_polls.getConnection(i).receiveRequestRaw();
			m_polls.updateConntectionFd(i);
			if (ret == 1){
				m_polls.getPollfdsAt(i).events |= POLLOUT;
				m_polls.getPollfdsAt(i).events &= ~POLLIN;
			} if (ret == -1) {
				m_polls.removeConnection(i--);
				continue;
			}
		}
		if (m_polls.getPollfdsAt(i).revents & POLLOUT)
		{
			LOG_INFO("Send response.")
			int error_code = m_polls.getConnection(i).sendResponse();
			m_polls.updateConntectionFd(i);
			if (m_polls.getConnection(i).isResponseCgi())
				m_polls.getPollfdsAt(i).events |= POLLIN;
			if (error_code == 1)
				continue;
			else if (error_code != 0)
			{
				std::cout << "Error: send\n";
				m_polls.removeConnection(i--);
				continue;
			}
			m_polls.getPollfdsAt(i).events |= POLLIN;
			m_polls.getPollfdsAt(i).events &= ~POLLOUT;
		}
		if (m_polls.getConnection(i).idleTime.isOver(1)) {
			LOGC(TERMC_DARKGREEN, ">>> Client time out <<<")
			m_polls.removeConnection(i--);
			continue;
		}
		m_polls.getPollfdsAt(i).revents = 0;
	}
	return (0);
}

int Webserver::runServer()
{
	int	pollRet = 0;
	if (m_servers.size() == 0)
	{
		std::cout << "NO servers\n";
		return (1);
	}
	while (1)
	{
		pollRet = poll(m_polls.getPollfds().data(), m_polls.getPollfds().size(), 100);
		if (!g_isRunning)
		{
			std::cout << "Stopping Server :)" << std::endl;
			
			return (0);
		}
		else if (pollRet == -1)
		{
			std::cout << "Error: poll error.\n";
			return (pollRet);
		}
		else if (pollRet == 0) {
			LOG_INFO("Poll timeout " << m_polls.getPollfds().size());
			removeTimeoutConnections();
		}
		else
		{
			LOGC(TERMC_ORANGE, pollRet << '/' << m_polls.getPollfds().size() - m_servers.size() << " Poll(s) triggered\n")
			pollServers();
			pollClients();
		}
	}
	return (pollRet);
}

void	Webserver::removeTimeoutConnections(void)
{
	for (std::size_t i = m_polls.getPollfds().size() - 1; i >= m_servers.size(); i--)
	{
		if (m_polls.getConnection(i).idleTime.isOver(1)) {
			LOGC(TERMC_DARKGREEN, ">>> Client time out <<<")
			m_polls.removeConnection(i);
			continue;
		}
	}
}

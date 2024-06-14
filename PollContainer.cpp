#include "PollContainer.hpp"

PollContainer::PollContainer(void) {}

PollContainer::PollContainer(const PollContainer& a_other) : m_pollfds(a_other.m_pollfds), m_connections(a_other.m_connections) {}

PollContainer& PollContainer::operator=(const PollContainer& a_other)
{
	if (this != &a_other)
	{
		m_connections = a_other.m_connections;
		m_pollfds = a_other.m_pollfds;
	}
	return (*this);
}

PollContainer::~PollContainer() {}

pollfd& PollContainer::getPollfdsAt(int a_idx)
{
	return (m_pollfds.at(a_idx));
}

std::vector<pollfd>& PollContainer::getPollfds(void)
{
	return (m_pollfds);
}

Connection& PollContainer::getConnection(int a_idx)
{
	return (m_connections.at(a_idx - (m_pollfds.size() - m_connections.size())));
}

void PollContainer::addServer(const Server& a_server)
{
	m_pollfds.push_back(newPollfd(a_server.getServerSocket(), POLLIN | POLLOUT | POLLERR | POLLNVAL));
}

void PollContainer::addConnection(const Connection& a_connection)
{
	m_pollfds.push_back(newPollfd(a_connection.getSocketFd(), POLLIN | POLLERR | POLLNVAL));
	m_connections.push_back(a_connection);
}

void PollContainer::removeConnection(int a_idx)
{
	close(m_pollfds.at(a_idx).fd);
	m_pollfds.erase(m_pollfds.begin() + a_idx);
	m_connections.erase(m_connections.begin() + a_idx - (m_pollfds.size() - m_connections.size() + 1));
}

pollfd PollContainer::newPollfd(int a_socket, int a_events) const
{
	pollfd	pfd;
	pfd.fd = a_socket;
	pfd.events = a_events;
	pfd.revents = 0;
	return (pfd);
}

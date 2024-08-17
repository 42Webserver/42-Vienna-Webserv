#ifndef POLLCONTAINER_HPP
# define POLLCONTAINER_HPP

#include <vector>
#include <sys/poll.h>
#include "Connection.hpp"
#include "Server.hpp"

class PollContainer
{
private:
	std::vector<pollfd>		m_pollfds;
	std::vector<Connection>	m_connections;
public:
	PollContainer(void);
	PollContainer(const PollContainer& a_other);
	PollContainer& operator=(const PollContainer& a_other);
	~PollContainer();

	pollfd&						getPollfdsAt(int a_idx);
	std::vector<pollfd>&		getPollfds(void);
	Connection&					getConnection(int a_idx);

	void	addServer(const Server& a_server);
	void	addConnection(const Connection& a_connection);
	void	removeConnection(int a_idx);
	void	updateConntectionFd(int a_idx);

	pollfd	newPollfd(int a_socket, int a_flags) const;
};

#endif // !POLLCONTAINER_HPP

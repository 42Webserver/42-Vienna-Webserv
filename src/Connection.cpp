#include "Connection.hpp"

Connection::Connection(Server& a_server, int a_clientSocket) : m_server(a_server), m_clientSocket(a_clientSocket), m_cgiFd(0), m_request(), m_response(m_request)
{
	// std::cout << "New connection on fd: " << m_clientSocket << '\n';
}

Connection::Connection(const Connection &a_other)
	: m_server(a_other.m_server), m_clientSocket(a_other.m_clientSocket), m_cgiFd(a_other.m_cgiFd), m_request(a_other.m_request), m_response(m_request, a_other.m_response) {}

Connection &Connection::operator=(const Connection &a_other)
{
	if (this != &a_other)
	{
		m_clientSocket = a_other.m_clientSocket;
		m_cgiFd = a_other.m_cgiFd;
		m_server = a_other.m_server;
		m_request = a_other.m_request;
		m_response = Response(m_request, a_other.m_response);
		idleTime = a_other.idleTime;
	}
	return (*this);
}

Connection::~Connection(void) {}

int	Connection::readAppend(std::string& a_appendString)
{
	static char	buffer[BUFFER_SIZE];
	int		ret;

	ret = recv(m_clientSocket, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	if (ret == -1)
		return (-1);
	a_appendString.append(buffer, ret);
	LOGC(TERMC_RED, "READ: " << ret << " BYTES")
	return (ret);
}

int	Connection::readHead()
{
	std::string	head;
	int			ret;

	if ((ret = readAppend(head)) == -1)
		return (-1);
	m_request.addHead(head);
	if (ret == 0)
		m_request.setHeadDone();
	return (0);
}

int	Connection::readBody()
{
	std::string bodyPart;
	int			ret;

	ret = readAppend(bodyPart);//, m_request.getContentLength()); //experimentell kann bei header mit Content-Length=99999999999999999999 sehr blöd sein
	if (ret == -1)
	{
		LOG_ERROR("This happened: ret: " << ret << " head:\n" << m_request.getHead() << "\nbody:\n" << m_request.getBody());
		return (-1);
	}
	m_request.addBody(bodyPart);
	if (ret == 0)
		m_request.setBodyDone();
	return (0);
}

int	Connection::getSocketFd(void) const
{
	return (m_cgiFd != 0 ? m_cgiFd : m_clientSocket);
}

int	Connection::receiveRequestRaw(void)
{
	if (m_response.isCgiResponse())
	{
		m_cgiFd = m_response.createResponseMsg();
		idleTime.resetTime();
		return (0);
	}
	if (!m_request.headComplete())
	{
		if (readHead())
			return (-1);
		if (m_request.headComplete())
		{
			if (m_request.getHead().length() == 0)
				return (-1);
			LOG("HEAD: \n" << m_request.getHead() << '\n')
			m_request.initMap();
			m_response = Response(m_request, m_server.getSubServer(m_request.getRequestHost()).getValidConfig(m_request.getValue("uri")));
			m_request.setMaxBodySize(m_response.getMaxBodySize());
		}
	}
	else if (!m_request.bodyComplete())
	{
		if (readBody())
			return (-1);
		LOG("Read Body");
	}
	idleTime.resetTime();
 	if (m_request.isReady())
		return (1);
	return (0);
}

int	Connection::sendResponse(void)
{
	int error_code = 0;
	if (!m_request.isReady())
		return (0);
	if ((m_cgiFd = m_response.createResponseMsg()))
	{
		idleTime.resetTime();
		return (1);
	}
	error_code = m_request.getIsValid();
	m_request = Request();
	const std::string	response = m_response.getResponse();
	m_response.clearBody();
	idleTime.resetTime();
	if (send(m_clientSocket, response.data(), response.size(), 0) == -1)
		return (-1);
	return (error_code);
}

bool	Connection::isResponseCgi(void) const
{
	return (m_cgiFd);
}

bool	Connection::operator==(const int a_fd) const
{
	return (m_clientSocket == a_fd);
}

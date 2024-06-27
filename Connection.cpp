#include "Connection.hpp"

Connection::Connection(Server& a_server, int a_clientSocket) : m_server(a_server), m_clientSocket(a_clientSocket), m_request(), m_response(m_request)
{
	// std::cout << "New connection on fd: " << m_clientSocket << '\n';
	m_idleStart = std::time(NULL);
}

Connection::Connection(const Connection &a_other)
	: m_server(a_other.m_server), m_clientSocket(a_other.m_clientSocket), m_idleStart(std::time(NULL)), m_request(a_other.m_request), m_response(m_request, a_other.m_response) {}

Connection &Connection::operator=(const Connection &a_other)
{
	if (this != &a_other)
	{
		m_clientSocket = a_other.m_clientSocket;
		m_server = a_other.m_server;
		m_idleStart = a_other.m_idleStart;
		m_request = a_other.m_request;
		m_response = Response(m_request, a_other.m_response);
	}
	return (*this);
}

Connection::~Connection(void) {}

int Connection::readAppend(std::string& a_appendString)
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

int Connection::readAppend(std::string& a_appendString, std::size_t a_size)
{
	std::vector<char> buffer(a_size);
	int		ret;

	ret = recv(m_clientSocket, buffer.data(), a_size, MSG_DONTWAIT);
	if (ret == -1)
		return (-1);
	a_appendString.append(buffer.begin(), buffer.begin() + ret);
	LOGC(TERMC_RED, "READ: " << ret << " BYTES")
	return (ret);
}

int Connection::readHead()
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

int Connection::readBody()
{
	std::string bodyPart;
	int			ret;

	ret = readAppend(bodyPart);//, m_request.getContentLength()); //experimentell kann bei header mit Content-Length=99999999999999999999 sehr blöd sein
	if (ret == -1)
	{
		LOG_ERROR("This happened: ret: " << ret << " head:\n" << m_request.getHead() << "\nbody:\n" << m_request.getBody());
		exit(42); //TO SEE IF IT EVER HAPPENS. !
		return (-1);
	}
	m_request.addBody(bodyPart);
	return (0);
}

int Connection::getSocketFd(void) const
{
	return (m_clientSocket);
}

int Connection::receiveRequestRaw(void)
{
	if (!m_request.headComplete())
	{
		if (readHead())
			return (-1);
		if (m_request.headComplete())
		{
			m_request.initMap();
			m_response = Response(m_request, m_server.getSubServer(m_request.getRequestHost()).getValidConfig(m_request.getValue("uri")));
			m_request.setMaxBodySize(m_response.getMaxBodySize());
		}
		LOG("HEAD: \n" << m_request.getHead() << '\n')
	}
	else if (!m_request.bodyComplete())
	{
		if (readBody())
			return (-1);
		LOG("Read Body");
		// LOG("BODY: \n" << m_request.getBody() << '\n')
	}
	m_idleStart = std::time(NULL);
 	if (m_request.isReady())
		return (1);
	return (0);
}

int Connection::sendResponse(void)
{
	int error_code = 0;
	if (!m_request.isReady())
		return (0);
	if (!m_response.createResponseMsg())
		return (1);
	error_code = m_request.getIsValid();
	m_request = Request();
	const std::string	response = m_response.getResponse();

	std::cout << "Response:\n" << response << '\n';
	m_idleStart = std::time(NULL);
	if (send(m_clientSocket, response.data(), response.size(), 0) == -1)
		return (-1);
	return (error_code);
}

time_t Connection::getIdleTime(void) const
{
	return (std::time(NULL) - m_idleStart);
}

bool Connection::operator==(const int a_fd) const
{
	return (m_clientSocket == a_fd);
}

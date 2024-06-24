#include "Connection.hpp"

Connection::Connection(Server& a_server, int a_clientSocket) : m_server(a_server), m_clientSocket(a_clientSocket), 
m_chunked(false)
{
	// std::cout << "New connection on fd: " << m_clientSocket << '\n';
	m_idleStart = std::time(NULL);
}

Connection::Connection(const Connection &a_other)
	: m_server(a_other.m_server), m_clientSocket(a_other.m_clientSocket), m_head(a_other.m_head), m_body(a_other.m_body), m_idleStart(std::time(NULL)), m_chunked(a_other.m_chunked)  {}

Connection &Connection::operator=(const Connection &a_other)
{
	if (this != &a_other)
	{
		m_clientSocket = a_other.m_clientSocket;
		m_head = a_other.m_head;
		m_body = a_other.m_body;
		m_server = a_other.m_server;
		m_idleStart = a_other.m_idleStart;
		m_chunked = a_other.m_chunked;
	}
	return (*this);
}

Connection::~Connection(void) {}

int Connection::readAppend(std::string& a_appendString)
{
	char	buffer[BUFFER_SIZE];
	int		ret;

	ret = recv(m_clientSocket, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	if (ret == -1)
		return (-1);
	a_appendString.append(buffer, ret);
	return (ret);
}

int Connection::readHead()
{
	std::size_t	sepPos;
	int			ret;

	if ((ret = readAppend(m_head)) == -1)
		return (-1);
	sepPos = m_head.find("\r\n\r\n");
	if (sepPos == std::string::npos)
	{
		std::cout << "OH NO?" << '\n';
		std::cout << "DEBUG: readHead: { ret: " << ret << "; m_head: [" << m_head << "]; }\n";
		return (-1);
	}
	m_body.append(m_head.begin() + sepPos + 4, m_head.end());
	m_head.erase(sepPos);
	return (0);
}

int Connection::readBody()
{
	std::size_t contentLength = m_request.getContentLength();
	int			ret;

	do
	{
		ret = readAppend(m_body);
		if (ret == -1)
		{
			std::cout << "DEBUG: readHead: { ret: " << ret << "; m_body: [" << m_body << "]; }\n";
			//exit(42); //TO SEE IF IT EVER HAPPENS. !
			return (-1);
		}
	} while(m_body.length() != contentLength && ret == BUFFER_SIZE);
	return (0);
}

int Connection::getSocketFd(void) const
{
	return (m_clientSocket);
}

int Connection::receiveRequestRaw(void)
{
	if (!m_chunked)
	{
		if (readHead())
			return (-1);
		m_request = Request(m_head);
		LOG("HEAD: \n" << m_head << '\n')
		m_head.clear();
	}
	if (m_body.size() || m_chunked)
	{
		if (readBody())
			return (-1);
		m_request.addBody(m_body);
		LOG("BODY: \n" << m_body << '\n')
		m_body.clear();
	}
	if (m_request.requestComplete())
	{
		m_response = Response(m_request, m_server.getSubServer(m_request.getRequestHost()).getValidConfig(m_request.getValue("uri")));
		m_response.createResponseMsg();
		m_chunked = false;
	}
	else
		m_chunked = true;
	m_idleStart = std::time(NULL);
	return (0);
}

int Connection::sendResponse(void)
{
	const std::string	response = m_response.getResponse();
	m_response.clearBody();
	//std::cout << "Response:\n" << response << '\n';
	m_idleStart = std::time(NULL);
	return (send(m_clientSocket, response.data(), response.size(), 0));
}

time_t Connection::getIdleTime(void) const
{
	return (std::time(NULL) - m_idleStart);
}

bool Connection::operator==(const int a_fd) const
{
	return (m_clientSocket == a_fd);
}

void Connection::printHeadNBody(void) const
{
	std::cout << "Head:\n" << m_head << '\n';
	std::cout << "Body:\n" << m_body << '\n';
}

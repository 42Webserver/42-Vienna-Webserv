#include "Request.hpp"

Request::Request(void) : m_isValid(true) {}

Request::Request(const Request &other)
{
	*this = other;
}

Request &Request::operator=(const Request &other)
{
	if (this != &other)
	{
		m_requestHeader = other.m_requestHeader;
		m_isValid = other.m_isValid;
	}
	return (*this);
}

Request::~Request() {}

Request::Request(std::string& head, std::string& body, int fd) : m_isValid(true), m_clientSocket(fd)
{
	(void)body;
	initMap(head);
}

void Request::getRequestLine(std::string& line)
{
	std::stringstream	input(line);
	std::string			value;

	if (line.empty())
	{
		std::cout << "EMPTY REQUEST HEADLINE!!!" << std::endl;
		m_isValid = false;
		return;
	}

	size_t	args = 0;
	while (std::getline(input, value, ' '))
	{
		if (args == 0)
			m_requestHeader["method"] = value;
		else if (args == 1)
			m_requestHeader["uri"] = value;
		else if (args == 2)
			m_requestHeader["http_version"] = value;
		args++;
	}
	if (args != 3)
	{
		std::cout << "INVALID AMOUNT OF ARGUMENTS IN REQUEST HEADLINE!!!" << std::endl;
		m_isValid = false;
		return;
	}
}

void Request::createKeyValuePair(std::string line)
{
	std::stringstream	input(line.substr(0, line.length()));

	if (line.empty())
	{
		std::cout << "EMPTY REQUEST LINE!!!" << std::endl;
		m_isValid = false;
		return;
	}

	std::string	arg;
	std::string	key;
	std::string	value;

	size_t pos = line.find(":");
	if (pos != std::string::npos)
	{
		key = line.substr(0, pos);
		if (pos != line.length() - 1)
			value = line.substr(pos + 2, line.length());
	}
	else
	{
		m_isValid = false;
		return;
	}
	m_requestHeader[key] = value;
}

void Request::initMap(std::string head)
{
	size_t	delimiter = head.find_first_of("\r\n");

	if (delimiter == std::string::npos)
	{
		std::cout << "INVALID REQUEST!!!" << std::endl;
		m_isValid = false;
		return;
	}

	std::string headline = head.substr(0, delimiter);
	std::string	remainder = head.substr(delimiter + 2, head.length() - delimiter);
	getRequestLine(headline);


	if (!m_isValid)
		return;

	std::string	line;
	size_t		pos, prevPos = 0;
	while ((m_isValid && (pos = remainder.find("\r\n", pos)) != std::string::npos))
	{
		line = remainder.substr(prevPos, pos - prevPos);
		if (line.empty())
			break;
		createKeyValuePair(line);
		pos += 2;
		prevPos = pos;
	}
	// for (std::map<std::string, std::string>::iterator it = m_requestHeader.begin(); it != m_requestHeader.end(); ++it)
	// 	std::cout << "key = '" << it->first << "' value = '" << it->second << "'" << std::endl;
}


int Request::getClientSocket() const
{
	return (m_clientSocket);
}

const std::string &Request::getValue(const std::string &a_key)
{
	return (m_requestHeader[a_key]);
}

std::string Request::getRequestHost() const
{
	std::string host;
	if (m_requestHeader.find("Host") != m_requestHeader.end())
	{
		host = m_requestHeader.at("Host");
		host = host.substr(0, host.find(':'));
	}
    return host;
}

bool	Request::getIsValid(void) const
{
	return (m_isValid);
}

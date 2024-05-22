#include "Request.hpp"

Request::Request(void)
{
}

Request::Request(const Request &other)
{
	*this = other;
}

Request &Request::operator=(const Request &other)
{
	if (this != &other)
	{
		m_requestHeader = other.m_requestHeader;
	}
	return (*this);
}

Request::~Request()
{
}



Request::Request(std::string& head, std::string& body, int fd) : m_clientSocket(fd)
{
	//std::istringstream iss(head);
	(void)body;
	initMap(head);
}

void Request::getRequestLine(std::string& line)
{
	std::istringstream input(line);
	std::string value;

	std::getline (input, value, ' ');
	m_requestHeader["method"] = value;
	std::getline (input, value, ' ');
	m_requestHeader["uri"] = value.substr(1);
	std::getline (input, value, '\r');
	m_requestHeader["http_version"] = value;
}

void Request::createKeyValuePair(std::string &line)
{
	std::istringstream input(line);
	std::string key;
	std::string value;

	std::getline (input, key, ':');
	std::getline (input, value, '\r');
	m_requestHeader[key] = value;
}

void Request::initMap(std::string head)
{
	std::string line;
	size_t pos = 0;
	size_t prevPos = 0;

	pos = head.find("\r\n", pos);
	pos += 2;
	line = head.substr(prevPos, pos - prevPos);
	getRequestLine(line);
	prevPos = pos;
	while ((pos = head.find("\r\n", pos)) != std::string::npos)
	{
		pos += 2;
		line = head.substr(prevPos, pos - prevPos);
		createKeyValuePair(line);
		prevPos = pos;
	}
	for (std::map<std::string, std::string>::iterator it = m_requestHeader.begin(); it != m_requestHeader.end(); ++it)
		std::cout << "key = '" << it->first << "' value = " << it->second  << std::endl;
}


int Request::getClientSocket() const
{
	return (m_clientSocket);
}

const std::string &Request::getValue(const std::string &a_key)
{
	return (m_requestHeader[a_key]);
}

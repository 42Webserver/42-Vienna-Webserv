#include "Request.hpp"

Request::Request(void) : m_isValid(0), m_headComplete(false), m_bodyComplete(false), m_chunkSize(0), m_maxBodySize(0) {}

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
		m_maxBodySize = other.m_maxBodySize;
		m_head = other.m_head;
		m_body = other.m_body;
		m_buffer = other.m_buffer;
		m_headComplete = other.m_headComplete;
		m_bodyComplete = other.m_bodyComplete;
		m_chunkSize = other.m_chunkSize;
		m_maxBodySize = other.m_maxBodySize;
	}
	return (*this);
}

Request::~Request() {}

void Request::getRequestLine(std::string& line)
{
	std::stringstream	input(line);
	std::string			value;

	if (line.empty())
	{
		std::cout << "EMPTY REQUEST HEADLINE!!!" << std::endl;
		m_isValid = 400;
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
		m_isValid = 400;
		return;
	}
}

void Request::createKeyValuePair(std::string line)
{
	std::stringstream	input(line.substr(0, line.length()));

	if (line.empty())
	{
		std::cout << "EMPTY REQUEST LINE!!!" << std::endl;
		m_isValid = 400;
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
		m_isValid = 400;
		return;
	}
	m_requestHeader[key] = value;
}

void Request::initMap()
{
	size_t	delimiter = m_head.find_first_of("\r\n");

	if (delimiter == std::string::npos)
	{
		std::cout << "INVALID REQUEST!!!" << std::endl;
		m_isValid = 400;
		return;
	}

	std::string headline = m_head.substr(0, delimiter);
	if (delimiter + 2 >= m_head.length())
	{
		m_isValid = 400;
		return;
	}
	std::string	remainder = m_head.substr(delimiter + 2, m_head.length() - delimiter);
	getRequestLine(headline);


	if (m_isValid)
		return;

	std::string	line;
	size_t		pos = 0, prevPos = 0;
	while ((!m_isValid && (pos = remainder.find("\r\n", pos)) != std::string::npos))
	{
		line = remainder.substr(prevPos, pos - prevPos);
		if (line.empty())
			break;
		createKeyValuePair(line);
		pos += 2;
		prevPos = pos;
	}
	if (m_requestHeader.find("Host") == m_requestHeader.end())
	{

		LOG_ERROR("Host is missing in Request Header");
		m_isValid = 400; 
	}
	addBody(""); //just so if we have added something to buffer in addHead, we will add it to body properly here.
	// for (std::map<std::string, std::string>::iterator it = m_requestHeader.begin(); it != m_requestHeader.end(); ++it)
	// 	std::cout << "key = '" << it->first << "' value = '" << it->second << "'" << std::endl;
}

void Request::setHeadDone(void)
{
	m_headComplete = true;
}

void Request::setBodyDone(void)
{
	m_bodyComplete = true;
}

void Request::setMaxBodySize(std::size_t a_maxBody)
{
	m_maxBodySize = a_maxBody;
	if (m_maxBodySize != 0 && m_body.length() > m_maxBodySize)
	{
		m_isValid = 413;
		m_body.clear();
	}
}

const std::string &Request::getValue(const std::string &a_key)
{
	return (m_requestHeader[a_key]);
}

void Request::setUri(std::string a_uri)
{
	m_requestHeader["uri"] = a_uri;
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

int	Request::getIsValid(void) const
{
	return (m_isValid);
}

size_t Request::getContentLength() const
{
	std::map<std::string, std::string>::const_iterator found = m_requestHeader.find("Content-Length");
	if (found != m_requestHeader.end())
	{
		//Maybe parse value, check for numbers!
		return (std::strtol(found->second.c_str(), NULL, 10));		
	}
	return 0;
}

void Request::addHead(const std::string &a_head)
{
	if (m_headComplete)
	{
		LOG_WARNING("DU MACHST WAS FALSCH?")
		return;
	}
	std::size_t	sepPos = a_head.find("\r\n\r\n");
	if (sepPos == std::string::npos)
	{
		m_headComplete = false;
		m_head.append(a_head);
		return ;
	}
	if (m_head.size() + sepPos + 4 > MAX_HEAD_SIZE)
	{
		LOG_ERROR("head too big");
		m_isValid = 431; //invalid request? header zu groß bzw müll
		return ;
	}
	m_head.append(a_head, 0, sepPos + 4);
	if (m_head.length() > MAX_HEAD_SIZE)
	{
		LOG_ERROR("head too big");
		m_isValid = 431; //invalid request? header zu groß bzw müll
		return ;
	}
	if (sepPos < a_head.length() - 4)
		m_buffer.append(a_head, sepPos + 4);
	m_headComplete = true;
}

bool Request::headComplete(void)
{
	return (m_headComplete || (m_headComplete = (m_head.rfind("\r\n\r\n") != std::string::npos)));
}

void Request::addBody(const std::string &a_body)
{
	if (m_maxBodySize > 0 && m_body.length() + a_body.length() >= m_maxBodySize)
	{
		m_isValid = 413;
		return ;
	}
	if (m_body.length() + a_body.length() >= MAX_BODY_SIZE) //? else if
	{
		m_isValid = 413;
		return ;
	}
	if (m_requestHeader["Transfer-Encoding"] == "chunked")
	{
		if (m_buffer.length())
		{
			m_buffer.append(a_body);
			std::size_t sepPos = m_buffer.rfind("\r\n");
			if (m_buffer.length() >= 1 + RN_CHAR_OFFSET && sepPos != std::string::npos && sepPos != 0)
			{
				if (m_buffer.at(0) == '\r' && m_buffer.at(1) == '\n')
					m_buffer.erase(0, 2);
				reciveChunked(m_buffer);
				m_buffer.clear();
			}
		}
		else
			reciveChunked(a_body);
		return ;
	}
	m_body.append(a_body);
}

void Request::reciveChunked(const std::string &a_body)
{
	std::cout << "Recieved data length: " << a_body.length() << '\n';
	std::size_t start = 0;
	std::size_t amount = 0;
	char *end_hex;
	while (start < a_body.length())
	{
		if (m_chunkSize == 0)
		{
			if (a_body.length() - start < 3)
			{
				m_buffer.append(a_body, start);
				return;
			}
			m_chunkSize = std::strtol(a_body.c_str() + start, &end_hex, BASE_HEX);
			if (m_chunkSize == 0)
			{
				if (a_body.c_str() + start == end_hex)
					LOG_ERROR("THERE IS SOMETHING WROOONG!");
				setBodyDone();
				break;
			}
			if (static_cast<std::size_t>((end_hex + RN_CHAR_OFFSET) - a_body.c_str()) <= a_body.length())
				start = (end_hex + RN_CHAR_OFFSET) - a_body.c_str();
			amount = m_chunkSize > a_body.length() - start ? a_body.length() - start : m_chunkSize;
		}
		else
			amount = m_chunkSize > a_body.length() - start ? a_body.length() - start : m_chunkSize;
		m_body.append(a_body, start, amount);
		m_chunkSize -= amount;
		start += amount;

		if (m_chunkSize == 0)
			start += RN_CHAR_OFFSET;
	}
}
/* {
	std::size_t	start = 0;
	std::size_t	amount = 0;
	std::size_t	rnPos = 0;
	char		*end_hex;
	while (start < a_body.length())
	{
		if (m_chunkSize == 0)
		{
			m_chunkSize = std::strtol(a_body.c_str() + start, &end_hex, BASE_HEX);
			if (m_chunkSize == 0)
			{
				setBodyDone();
				return;
			}
			if (static_cast<std::size_t>((end_hex + RN_CHAR_OFFSET) - a_body.c_str()) < a_body.length())
				start = (end_hex + RN_CHAR_OFFSET) - a_body.c_str();
		}
		rnPos = a_body.find("\r\n");
		amount = rnPos != m_chunkSize ? a_body.length() : m_chunkSize;
		m_body.append(a_body, start, amount);
	}
} */

bool Request::bodyComplete(void)
{
	return (m_bodyComplete || (getContentLength() == m_body.length() && m_requestHeader["Transfer-Encoding"] != "chunked"));
}

bool Request::isReady(void)
{
	return ((headComplete() && bodyComplete()) || m_isValid);
}

const std::string &Request::getHead()
{
	return (m_head);
}

const std::string &Request::getBody()
{
	return (m_body);
}

bool Request::getValue(const std::string &a_key, std::string &a_returnValue) const
{
	std::map<std::string, std::string>::const_iterator found = m_requestHeader.find(a_key);
	if (found != m_requestHeader.end()) {
		a_returnValue = found->second;
		return (true);
	}
	return (false);
}

void Request::setValue(const std::string &a_key, const std::string &a_val)
{
	m_requestHeader[a_key] = a_val;
}

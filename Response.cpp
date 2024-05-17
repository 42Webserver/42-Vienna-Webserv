#include "Response.hpp"

Response::Response(void)
{
}

Response::Response(const Dummy_Request& a_request) : m_request(a_request)
{
}

Response::Response(const Response &other)
{
	*this = other;
}

Response &Response::operator=(const Response &other)
{
	if (this != &other)
	{
		m_request = other.m_request;
		m_responseMsg = other.m_responseMsg;
	}
	return (*this);
}

Response::~Response()
{
}

void Response::createResponseMessage()
{
	std::cout << m_request.getValue("method") << '\n';
}

int Response::checkMethod()
{
	std::string method = m_request.getValue("method");
	if (method.empty())
		return (405); //Something else if there is no method???
	if (method == "GET" || method == "POST" || method == "DELETE")
		return 0;
	else
		return (405); //Errorcode Method not Found
}

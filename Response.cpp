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
/* 	int error_code;
	if ((error_code = checkMethod()) > 0) */
		
}

int Response::checkMethod()
{
	std::string method = m_request.getValue("method");

	if (method.empty())
		return (405); //Something else if there is no method???
	else if (method == "GET" || method == "POST" || method == "DELETE")
		return 0;
	else
		return (405); //Errorcode Method not Found
}

int Response::checkUri()
{
	if (m_request.getValue("method") == "GET")
	{
		DIR* directory;
		struct dirent *readDir;
		std::string uri = m_request.getValue("uri");

		if (uri.empty())
			return (std::cerr << "Error: empty uri" << '\n', 400); //400 Bad Request
		if (uri.length() > 256)
			return (std::cerr << "Error: Uri too long" << '\n', 414); //414 Uri too long
		directory = opendir("www");
		if (!directory)
			return (std::cerr << "Error: directory not found" << '\n', 500); //500 Internal Server 
		while ((readDir = readdir(directory)) != NULL)
		{
			if (m_request.getValue("uri") == readDir->d_name)
				return (std::cout << "Filename found!!!!	", 0); //File found
		}
		return (404); //File not found!
	}
	return (0);
}

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
	int error_code;
	if ((error_code = checkMethod()) > 0)
	{
		sendErrorMsg(error_code);
		return ;
	}
	if ((error_code = checkUri()) > 0)
	{
		sendErrorMsg(error_code);
		return ; 
	}
	if ((error_code = checkHttpVersion()) > 0)
	{
		sendErrorMsg(error_code);
		return ;
	}
	if (error_code == 0)
	{
		sendValidMsg(200);
		return ;
	}
	//Read from file to string for uploading page!
}

void Response::sendValidMsg(int const & a_error_code)
{
	getResponseHeader(a_error_code);
	getBody("www/index.html");
	std::cout << m_responseMsg << '\n';
}

void Response::sendErrorMsg(int const & a_error_code)
{
	getResponseHeader(a_error_code);
	getBody("error/notFound.html");
	std::cout << m_responseMsg << '\n';
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
			{
				closedir(directory);
				return (/* std::cout << "Filename found!!!!" << '\n', */ 0); //File found
			}
		}
		closedir(directory);
		return (404); //File not found!
	}
	return (0);
}

int Response::checkHttpVersion()
{
    if (m_request.getValue("http_version") != "HTTP/1.1")
		return (505); // 505 Version not supported
	return 0;
}

void Response::getResponseHeader(int const & a_status_code)
{
	std::string response_header;

	addStatusLine(a_status_code, response_header);
	addDateAndTime(response_header);
	addServerName(response_header);
	addServerConnection(response_header);
	response_header.append("\r\n");
	m_responseMsg += response_header;
}

void	Response::addStatusLine(int const &a_status_code, std::string& a_response_header)
{
	std::ostringstream convert;

	convert << a_status_code;
	a_response_header.append(m_request.getValue("http_version"));
	a_response_header += ' ';
	a_response_header.append(convert.str());
	a_response_header += ' ';
	a_response_header.append(g_status_codes[convert.str()]);
	a_response_header.append("\r\n");
	
}

void Response::addServerName(std::string &a_response_header)
{
	a_response_header.append("Server: ");
	a_response_header.append("Webserver"); //set dynamic!
	a_response_header.append("\r\n");
}

void Response::addDateAndTime(std::string &a_response_header)
{
	std::time_t t = std::time(NULL);
    std::tm* now = std::localtime(&t);

	char buffer[30];
	a_response_header.append("Date: ");
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", now);
	
	a_response_header.append(buffer);
	a_response_header.append("\r\n");
}

void Response::addServerConnection(std::string &a_response_header)
{
	//check if we are sending, more response message => keep-alive!
	//else closed
	a_response_header.append("Connection: closed");
	a_response_header.append("\r\n");
}

std::string const &Response::getReponse() const
{
	return (m_responseMsg);
}

void Response::getBody(std::string const &filename)
{
	std::ifstream input_file(filename.c_str());
	std::stringstream body;
	
	if (!input_file.is_open() || !input_file.good())
	{
		std::cerr << "Error: open error file" << '\n';
		return ;
	}
	body << input_file.rdbuf();
	m_responseMsg.append(body.str());
	m_responseMsg.append("\r\n");
}

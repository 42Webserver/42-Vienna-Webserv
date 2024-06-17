#include "Response.hpp"

std::map<std::string, std::string>	Response::s_status_codes;

Response::Response() {}

Response::Response(const Request& a_request, const t_config& a_config) : m_request(a_request), m_config(a_config)
{

/* 			for (std::map<std::string, std::vector<std::string> > ::iterator it = m_config.begin(); it != m_config.end(); ++it)
			{
				std::cout << "		Key: " << it->first << " | value: ";
				for (size_t j = 0; j < it->second.size(); j++)
				{
					std::cout << it->second.at(j) << ", ";
				}
				std::cout<< std::endl;
			} */
			std::cout << "STATUS CODE =";
			for (std::map<std::string, std::string>::iterator it = s_status_codes.begin(); it != s_status_codes.end(); ++it)
			{
				std::cout << it->first + ": " + it->second + "\n";
			}
			
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
		m_responseHeader = other.m_responseHeader;
		m_responseBody = other.m_responseBody;
		m_config = other.m_config;
	}
	return (*this);
}

Response::~Response()
{
}

void Response::initStatusCodes()
{
	s_status_codes["200"] = "OK";
    s_status_codes["400"] = "Bad Request";
    s_status_codes["404"] = "Not Found";
    s_status_codes["405"] = "Method Not Allowed";
    s_status_codes["414"] = "URI Too Long";
    s_status_codes["500"] = "Internal Server Error";
    s_status_codes["505"] = "HTTP Version not supported";

	s_status_codes["000"] = "LANDING PAGE!";
}

const std::string &Response::getResponse() const
{
	return (m_responseHeader + m_responseBody);
}



void Response::setErrorMsg(const std::string &a_status_code)
{
	getResponseHeader(a_status_code);
	std::map<std::string, std::vector<std::string> >::iterator found = m_config.find(a_status_code);
	if (found == m_config.end())
		setDefaultErrorMsg(a_status_code);
	//else 
		//readFromFile(found->second);
	//Read from custom error page!;
}

void Response::setDefaultErrorMsg(const std::string &a_status_code)
{
	m_responseBody.append("<!DOCTYPE html><html><title>");
	m_responseBody.append(s_status_codes[a_status_code]);
	m_responseBody.append("</title><h1>");
	m_responseBody.append(a_status_code + " " + s_status_codes[a_status_code]);
	m_responseBody.append("</h1></html>\r\n");
}

	

void Response::createResponseMsg()
{
	if (!m_request.getIsValid())
		setErrorMsg("400");
	
	setErrorMsg("404");
}

//////////////////////+++++Response Header+++++++++++//////////////////////

void Response::getResponseHeader(const std::string &a_status_code)
{
	std::string response_header;

	addStatusLine(a_status_code, response_header);
	addDateAndTime(response_header);
	addContentLength(response_header);
	response_header.append("\r\n");
	m_responseHeader += response_header;
}

void	Response::addStatusLine(const std::string &a_status_code, std::string& a_response_header)
{
	a_response_header.append(m_request.getValue("http_version"));
	a_response_header += ' ';
	a_response_header.append(a_status_code);
	a_response_header += ' ';
	a_response_header.append(s_status_codes[a_status_code]);
	a_response_header.append("\r\n");
}

void Response::addDateAndTime(std::string &a_response_header)
{
	std::time_t t = std::time(NULL);
    std::tm* now = std::localtime(&t);

	char buffer[32];
	a_response_header.append("Date: ");
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S CEST", now);

	a_response_header.append(buffer);
	a_response_header.append("\r\n");
}

void Response::addContentLength(std::string &a_response_header)
{
	std::ostringstream convert;

	convert << m_responseBody.length();
	a_response_header.append("Content-Length: ");
	a_response_header.append(convert.str());
	a_response_header.append("\r\n");
}
//check is Request is valid? => if (false ) ? badRequest : weiter
//check httpVersion! 
//check check and set uri!
//check method
//get Method
// check if return => if (true) ? return statuscode and redirection with key Location:
// check valid root
//	check autoindex if (true) ? root => displayen directory tree : send index
// send index: file 


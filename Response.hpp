#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <dirent.h>
#include <ctime>
#include "Request.hpp"


/// global variable///
extern std::map<std::string, std::string> g_status_codes;

class Response
{
private:

	std::string	m_responseMsg;
	Request		m_request;

public:

	Response(void);
	Response(const Request& a_request);
	Response(const Response& other);
	Response& operator=(const Response& other);
	~Response();

	void				createResponseMessage();
	void				sendErrorMsg(int const & a_error_code);
	void				sendValidMsg(int const & a_error_code);
	int					checkMethod();
	int					checkUri();
	int					checkHttpVersion();
	void				getResponseHeader(int const & a_status_code, const std::string& a_body);
	void				addContentLength(const std::string& a_body, std::string& a_response_header);
	void				addStatusLine(int const & a_status_code, std::string& a_reponse_header);
	void				addServerName(std::string& a_response_header);
	void				addDateAndTime(std::string& a_response_header);
	void				addServerConnection(std::string& a_response_header);
	std::string const & getResponse() const;
	std::string 		getBody(std::string const & filename);
	std::string			getDefaultErrorBody(int const& a_status_code);
};

#endif // !RESPONSE_HPP

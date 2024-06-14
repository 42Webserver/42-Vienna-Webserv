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
#include "Data.hpp"
#include "ConfigParser.hpp"


/// global variable///
extern std::map<std::string, std::string> g_status_codes;

class Response
{
private:

	std::string			m_responseMsg;
	Request				m_request;
	struct subserver	m_subServer;

public:

	Response(void);
	Response(const Request& a_request, const subserver& a_subserver);
	Response(const Response& other);
	Response& operator=(const Response& other);
	~Response();

	std::string const & getResponse() const;
	void				getBody(std::string const & filename);

private:

	void				createResponseMessage();
	void				buildErrorMsg(int const & a_error_code);
	void				buildValidMsg(int const & a_error_code);
	int					checkMethod();
	int					checkUri();
	int					checkHttpVersion();
	void				getResponseHeader(int const & a_status_code);
	void				addStatusLine(int const & a_status_code, std::string& a_reponse_header);
	void				addServerName(std::string& a_response_header);
	void				addDateAndTime(std::string& a_response_header);
	void				addServerConnection(std::string& a_response_header);
};

#endif // !RESPONSE_HPP

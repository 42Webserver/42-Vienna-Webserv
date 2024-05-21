#ifndef RESPONSE_H
# define RESPONSE_H

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <dirent.h>
#include <ctime>
#include "Dummy_Request.hpp" 


/// global variable///
extern std::map<std::string, std::string> g_status_codes;

class Response
{
private:

	std::string     m_responseMsg;
	Dummy_Request   m_request;

public:

	Response(void);
	Response(const Dummy_Request& a_request);
	Response(const Response& other);
	Response& operator=(const Response& other);
	~Response();

	void				createResponseMessage();
	void				sendErrorMsg(int const & a_error_code);
	int					checkMethod();
	int					checkUri();
	int					checkHttpVersion();
	void				getResponseHeader(int const & a_status_code);
	void				addStatusLine(int const & a_status_code, std::string& a_reponse_header);
	void				addServerName(std::string& a_response_header);
	void				addDateAndTime(std::string& a_response_header);
	void				addServerConnection(std::string& a_response_header);
	std::string const & getReponse() const;

};

#endif // !RESPONSE_H

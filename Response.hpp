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

	std::string									m_responseHeader;
	std::string									m_responseBody;
	Request										m_request;
	t_config									m_config;
	static std::map<std::string, std::string>	s_status_codes;

	void 	getResponseHeader(const std::string &a_status_code);
	void	addStatusLine(const std::string &a_status_code, std::string& a_response_header);
	void	addDateAndTime(std::string &a_response_header);
	void	setErrorMsg(const std::string &a_status_code);
	void	setDefaultErrorMsg(const std::string &a_status_code);
	void	addContentLength(std::string &a_response_header);
	

public:

	Response(void);
	Response(const Request& a_request, const t_config& a_subserver);
	Response(const Response& other);
	Response& operator=(const Response& other);
	~Response();

	const std::string&	getResponse() const;
	void	createResponseMsg();
	static void initStatusCodes();

};

#endif // !RESPONSE_HPP

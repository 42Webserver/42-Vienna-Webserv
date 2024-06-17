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
	t_config			m_config;

public:

	Response(void);
	Response(const Request& a_request, const t_config& a_subserver);
	Response(const Response& other);
	Response& operator=(const Response& other);
	~Response();

private:

};

#endif // !RESPONSE_HPP

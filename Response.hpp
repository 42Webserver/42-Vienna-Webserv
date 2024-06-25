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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


#define SERVERNAME "Surfing in the web (Ubuntu)"

class Response
{
private:

	std::string									m_responseHeader;
	std::string									m_responseBody;
	Request&									m_request;
	t_config									m_config;
	static std::map<std::string, std::string>	s_status_codes;
	static std::map<std::string, std::string>	s_content_type;

	void 		getResponseHeader(const std::string &a_status_code, const std::string &a_redirLoc, const std::string &a_content_type);
	void		addStatusLine(const std::string &a_status_code, std::string& a_response_header);
	void		addDateAndTime(std::string &a_response_header);
	void		setErrorMsg(const int &a_status_code);
	void		setDefaultErrorMsg(const std::string &a_status_code);
	void		setValidMsg(const std::string &filepath);
	void		addContentLength(std::string &a_response_header);
	int			checkHeaderline();
	bool		getBody(std::string const &filename);
	bool		checkAllowedMethod();
	int			isValidFile(std::string &a_filepath);
	int			getValidFilePath(std::string &a_filepath);
	bool		checkReturnResponse();
	void		addRedirection(std::string &a_response_header, const std::string &redLoc);
	void		addServerName(std::string &a_response_header);
	void		addContentType(std::string &a_response_header, const std::string &a_content_type);
	std::string	getFileType(const std::string &filepath);
	void    createAutoIndex(std::string &a_path);

	Response(void);

public:

	Response(Request& a_request);
	Response(Request& a_request, const t_config& a_subserver);
	Response(Request& a_request, const Response& other);
	Response(const Response& other);
	Response& operator=(const Response& other);
	~Response();

	static void	initContentType();
	static void	initStatusCodes();
	const std::string	getResponse() const;
	void	createResponseMsg();
	void	clearBody();
};

#endif // !RESPONSE_HPP

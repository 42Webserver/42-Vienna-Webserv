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
#include "CGI.hpp"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "SharedPtr.hpp"

#define SERVERNAME "Surfing in the web (Ubuntu)"

#define	REDIRECTION 1U << 0		//WIR HABEN EINE RETURN VALUE;
#define	REDIR_LOCATION 1U << 1	//WIR HABEN EINE LOCATION MIT ANGEGEBEN AUS CONFIG
#define REDIR_DIRECTORY 1U << 2	//REQUEST IST EIN DIRECTORY!

#define BYTE_TO_KB 1000

class Response
{
private:

	static std::map<std::string, std::string>	s_status_codes;
	static std::map<std::string, std::string>	s_content_type;

	std::string									m_responseHeader;
	std::string									m_responseBody;
	Request&									m_request;
	t_config									m_config;
	unsigned int								m_eventFlags;
	SharedPtr<CGI>								m_cgi;


	void 		getResponseHeader(const std::string &a_status_code, const std::string &a_redirLoc, const std::string &a_content_type);
	void		addStatusLine(const std::string &a_status_code, std::string& a_response_header);
	void		addDateAndTime(std::string &a_response_header);
	void		addConnection(const std::string& a_status_code, std::string& a_response_header);
	void		setErrorMsg(const int &a_status_code);
	void		setDefaultErrorMsg(const std::string &a_status_code);
	void		setValidMsg(const std::string &filepath);
	void		addContentLength(std::string &a_response_header);
	int			checkHeaderline();
	bool		getBody(std::string const &filename);
	bool		checkAllowedMethod();
	int			isValidFile(std::string &a_filepath);
	int			getValidFilePath(std::string &a_filepath);
	std::string	decodeUri(const std::string& a_uri, std::string& a_query);
	int			isReturnResponse();
	void		addRedirection(std::string &a_response_header, const std::string &redLoc);
	void		addServerName(std::string &a_response_header);
	void		addContentType(std::string &a_response_header, const std::string &a_content_type);
  	void    	createAutoIndex(std::string &a_path);
	std::string	getFileType(const std::string &filepath);
	int			isValidRequestHeader();
	bool		isCgiReady();

	Response(void);

public:

	Response(Request& a_request);
	Response(Request& a_request, const t_config& a_subserver);
	Response(Request& a_request, const Response& other);
	Response(const Response& other);
	Response& operator=(const Response& other);
	~Response();

	bool		isCgiResponse();
	static void	initContentType();
	static void	initStatusCodes();
	const std::string	getResponse() const;
	bool	createResponseMsg();
	void	clearBody();
	std::size_t	getMaxBodySize(void) const;
};

#endif // !RESPONSE_HPP

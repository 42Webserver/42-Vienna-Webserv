#ifndef CGI_HPP
# define CGI_HPP

#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "Data.hpp"
#include "Request.hpp"
#include "Timer.hpp"

#define PATH_INFO_STRING "PATH_INFO="
#define SERVER_PROTOCOL_STRING "SERVER_PROTOCOL=HTTP/1.1"
#define PATH_INFO_STRING_LENGTH 10
#define CGI_TIMEOUT_SECONDS 15

class CGI
{
private:

	char*						m_path;
	std::vector<char*>			m_argv;
	std::vector<char*>			m_envp;
	t_config					m_config;
	Request&					m_request;
	int							m_pid;
	int							m_inputPipe[2];
	int							m_outputPipe[2];
	int							m_status;
	std::string					m_responseBody;
	std::string					m_filePath;

	enum
	{
		CGI_WRITING = 1,
		CGI_CLIENTFD,
		CGI_READING,
		CGI_FINISHED
	} m_cgiStatus;

	CGI(void);
	CGI(const CGI& other);
	CGI& operator=(const CGI& other);

	int		scriptIsExecutable(const std::string& a_filePath) const;
	int		setPath(const std::string& a_filePath);
	int		run();
	void	setArgv(const std::string& a_filePath);
	void	setEnvp();
	void	deleteData();

public:

	Timer				startTime;

	CGI(t_config config, Request& a_request);
	~CGI();

	int					execute(std::string a_filePath);
	int					io(void);
	int					getFd(void) const;
	int					getStatusCode() const;
	void				setPathInfo(const std::string& a_pathInfo);
	void				setUrlQuery(const std::string& a_urlQuery);
	const std::string& 	getResponseBody() const;
};

#endif // !CGI_HPP

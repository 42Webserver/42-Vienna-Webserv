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

class CGI
{
private:

	char*						m_path;
	char**						m_argv;
	char**						m_envp;

	t_config					m_config;
	Request						m_request;

	std::string					m_responseBody;

	CGI(void);
	CGI(const CGI& other);
	CGI& operator=(const CGI& other);

	int		scriptIsExecutable();
	int		setPath();
	int		run();
	void	setArgv();
	void	setEnvp();
	void	deleteData();

public:

	CGI(t_config config, Request m_request);
	~CGI();

	int					execute();
	const std::string& 	getResponseBody() const;
};

#endif // !CGI_HPP

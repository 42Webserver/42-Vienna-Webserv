#ifndef CGI_HPP
# define CGI_HPP

#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
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


	CGI(void);
	CGI(const CGI& other);
	CGI& operator=(const CGI& other);

	int		scriptIsExecutable();
	int		setPath();
	void	setArgv();
	void	setEnvp();

public:

	CGI(t_config config, Request m_request);
	~CGI();

	int	execute();
};

#endif // !CGI_HPP

#ifndef CGI_HPP
# define CGI_HPP

#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

class CGI
{
private:

	std::string					m_executorPath;
	std::string					m_scriptPath;
	std::vector<const char*>	m_args;
	std::vector<const char*>	m_env;


public:

	CGI(void);
	CGI(const CGI& other);
	CGI& operator=(const CGI& other);
	~CGI();

	void	execute(const std::string filename);
	void	getExecutorPath(const std::string filename);
};

#endif // !CGI_HPP

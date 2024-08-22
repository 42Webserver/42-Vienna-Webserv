#include "CGI.hpp"

CGI::CGI(t_config config, Request& request) : m_path(NULL), m_config(config), m_request(request), m_pid(0), m_status(0), m_sentBytes(0), m_cgiStatus(CGI_WRITING)
{
	m_inputPipe[0] = -1;
	m_inputPipe[1] = -1;
	m_outputPipe[0] = -1;
	m_outputPipe[1] = -1;
}

CGI::CGI(const CGI &other) 
	:  m_config(other.m_config), m_request(other.m_request), m_pid(other.m_pid), m_status(other.m_status), m_sentBytes(0), m_cgiStatus(other.m_cgiStatus)
{
	m_inputPipe[0] = other.m_inputPipe[0];
	m_inputPipe[1] = other.m_inputPipe[1];
	m_outputPipe[0] = other.m_outputPipe[0];
	m_outputPipe[1] = other.m_outputPipe[1];

	m_path = new char[std::strlen(other.m_path) + 1];
	std::strcpy(m_path, other.m_path);

	for (size_t i = 0; i < other.m_argv.size(); i++)
	{
		char* str = new char[std::strlen(other.m_argv.at(i)) + 1];
		std::strcpy(str, other.m_argv.at(i));
		m_argv.push_back(str);
	}
	for (size_t i = 0; i < other.m_envp.size(); i++)
	{
		char* str = new char[std::strlen(other.m_envp.at(i)) + 1];
		std::strcpy(str, other.m_envp.at(i));
		m_envp.push_back(str);
	}
}

CGI& CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		deleteData();

		m_config = other.m_config;
		m_pid = other.m_pid;
		m_inputPipe[0] = other.m_inputPipe[0];
		m_inputPipe[1] = other.m_inputPipe[1];
		m_outputPipe[0] = other.m_outputPipe[0];
		m_outputPipe[1] = other.m_outputPipe[1];
		m_status = other.m_status;
		m_sentBytes = other.m_sentBytes;
		m_cgiStatus = other.m_cgiStatus;

		m_path = new char[std::strlen(other.m_path) + 1];
		std::strcpy(m_path, other.m_path);
		for (size_t i = 0; i < other.m_argv.size(); i++)
		{
			char* str = new char[std::strlen(other.m_argv.at(i)) + 1];
			std::strcpy(str, other.m_argv.at(i));
			m_argv.push_back(str);
		}
		for (size_t i = 0; i < other.m_envp.size(); i++)
		{
			char* str = new char[std::strlen(other.m_envp.at(i)) + 1];
			std::strcpy(str, other.m_envp.at(i));
			m_envp.push_back(str);
		}
	}
	return (*this);
}

CGI::~CGI() 
{
	deleteData();
	if (m_inputPipe[0] > 2)
		close(m_inputPipe[0]);
	if (m_inputPipe[1] > 2)
		close(m_inputPipe[1]);
	if (m_outputPipe[0] > 2)
		close(m_outputPipe[0]);
	if (m_outputPipe[1] > 2)
		close(m_outputPipe[1]);
}

int	CGI::scriptIsExecutable(const std::string& a_filePath) const
{
	return (!access(a_filePath.c_str(), X_OK) ? 0 : 500);
}

int	CGI::setPath(const std::string& a_filePath)
{
	t_config::iterator extentions = m_config.find("extension");
	if (extentions == m_config.end())
		return (500);
	size_t i = 0;
	for (; i < extentions->second.size(); ++i)
	{
		if (a_filePath.find(extentions->second.at(i)) == a_filePath.length() - extentions->second.at(i).length())
			break;
	}
	if (i >= extentions->second.size())
	{
		LOG_ERROR("CGI: extension not found");
		return (500);
	}
	if (m_config.find("script_path") == m_config.end() || m_config.at("script_path").size() <= i)
		return (500);
	m_path = new char[m_config.at("script_path").at(i).length() + 1];
	std::strcpy(m_path, m_config.at("script_path").at(i).c_str());
	return (0);
}

void	CGI::setArgv(const std::string& a_filePath)
{
	m_argv.push_back(new char[std::strlen(m_path) + 1]);
	std::strcpy(m_argv[0], m_path);

	m_argv.push_back(new char[a_filePath.length() + 1]);
	std::strcpy(m_argv[1], a_filePath.c_str());
}

void	CGI::setEnvp()
{
	std::vector<std::string>	vars;
	std::stringstream ss;

	ss << "CONTENT_LENGTH=" << m_request.getBody().length();
	vars.push_back("REQUEST_METHOD=" + m_request.getValue("method"));
	vars.push_back("CONTENT_TYPE=" + m_request.getValue("Content-Type"));
	vars.push_back(ss.str());
	vars.push_back(SERVER_PROTOCOL_STRING);
	vars.push_back("HTTP_COOKIE=" + m_request.getValue("Cookie"));
	if (m_config.at("upload").size() == 1)
		vars.push_back("UPLOAD="+ m_config.at("upload").at(0));

	size_t	i = 0;
	while (i < vars.size())
	{
		char *str = new char[vars.at(i).length() + 1];

		std::strcpy(str, vars.at(i).c_str());
		m_envp.push_back(str);
		i++;
	}
}

int	CGI::run()
{
  	if (pipe(m_inputPipe) == -1 || pipe(m_outputPipe) == -1)
	{
		std::cout << "Error while opening pipe\n";
		return (500);
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		std::cout << "Error while forking\n";
		return (500);
	}
 	if (pid == 0)
	{
		bool error = false;
		if (dup2(m_outputPipe[1], STDOUT_FILENO) == -1 ||
			dup2(m_inputPipe[0], STDIN_FILENO) == -1 ||
			dup2(m_outputPipe[1], STDERR_FILENO) == -1)
		{
			error = true;
		}
		close(m_outputPipe[0]);
		close(m_outputPipe[1]);
		close(m_inputPipe[0]);
		close(m_inputPipe[1]);
		if (error)
			exit(42);
		m_envp.push_back(NULL);
		m_argv.push_back(NULL);
		if (chdir(m_filePath.substr(0, m_filePath.find_last_of('/')).c_str()) == -1)
			exit(42);
		if (execve(m_path, m_argv.data(), m_envp.data()) == -1)
			exit(42);
		exit(0);
	}
	else
	{
		m_pid = pid;
	}
	return (0);
}

void	CGI::deleteData()
{
	if (m_path != NULL)
	{
		delete[] (m_path);
		m_path = NULL;
	}
	for (size_t i = 0; i < m_argv.size(); i++)
	{
		if (m_argv.at(i) != NULL)
			delete[] (m_argv.at(i));
	}
	m_argv.clear();
	for (size_t i = 0; i < m_envp.size(); i++)
	{
		if (m_envp.at(i) != NULL)
			delete[] (m_envp.at(i));
	}
	m_envp.clear();
}

int	CGI::io()
{
	if (startTime.isOver(CGI_TIMEOUT_SECONDS))
	{
		kill(m_pid, SIGINT);
		m_status = 500;
		close(m_inputPipe[0]);
		close(m_inputPipe[1]);
		close(m_outputPipe[0]);
		close(m_outputPipe[1]);
		m_inputPipe[0] = -1;
		m_inputPipe[1] = -1;
		m_outputPipe[0] = -1;
		m_outputPipe[1] = -1;
		return (0);
	}

	if (m_cgiStatus == CGI_WRITING)
	{
		ssize_t n = write(
			m_inputPipe[1],
			m_request.getBody().c_str() + m_sentBytes, 
			m_request.getBody().length() - m_sentBytes > 5000 ? 5000 : m_request.getBody().length() - m_sentBytes);
		if (n == -1)
		{
			m_status = 500;
			m_cgiStatus = CGI_FINISHED;
			return (0);
		}
		m_sentBytes += n;
		if (m_sentBytes == m_request.getBody().length() || n == 0)
		{
			close(m_inputPipe[1]);
			close(m_inputPipe[0]);
			m_inputPipe[1] = -1;
			m_sentBytes = 0;
			m_cgiStatus = CGI_CLIENTFD;
		}
	}
	else if (m_cgiStatus == CGI_CLIENTFD)
	{
		int status_code = 0;
		if (waitpid(m_pid, &status_code, WNOHANG) == 0)
		{
			return (-1);
		}
		if (WIFEXITED(status_code) && WEXITSTATUS(status_code) != 0)
		{
			m_status = 500;
			if (WEXITSTATUS(status_code) == 42)
			{
				close(m_outputPipe[0]);
				close(m_outputPipe[1]);
				m_cgiStatus = CGI_FINISHED;
				return (0);
			}
		}
		m_cgiStatus = CGI_READING;
	}
	else if (m_cgiStatus == CGI_READING)
	{
		ssize_t n = 0;
		char buffer[4096];
		
		n = read(m_outputPipe[0], buffer, 4096);
		if (n == -1)
		{
			m_status = 500;
			m_cgiStatus = CGI_FINISHED;
			return (0);
		}
		if (n > 0)
			m_responseBody.append(buffer, n);
		if (n < 4096)
		{
			close(m_outputPipe[0]);
			close(m_outputPipe[1]);
			m_outputPipe[0] = -1;
			m_cgiStatus = CGI_FINISHED;
			return (0);
		}
	}
	else
		return (0);
	return (-1);
}

int	CGI::getFd(void) const
{
	if (m_cgiStatus == CGI_WRITING)
		return (m_inputPipe[1]);
	if (m_cgiStatus == CGI_CLIENTFD)
		return (-1);
	if (m_cgiStatus == CGI_READING)
		return (m_outputPipe[0]);
	return (0);
}

const std::string&	CGI::getResponseBody() const
{
	return (m_responseBody);
}

int	CGI::getStatusCode() const
{
	return (m_status);
}

void	CGI::setUrlQuery(const std::string &a_urlQuery)
{
	std::string envvar = "QUERY_STRING=";
	envvar.append(a_urlQuery);
	char* str = new char[envvar.length() + 1];
	std::strcpy(str, envvar.c_str());
	m_envp.push_back(str);
}

void	CGI::setPathInfo(const std::string &a_pathInfo)
{
	char* str = new char[a_pathInfo.length() + PATH_INFO_STRING_LENGTH + 1];
	std::strcpy(str, PATH_INFO_STRING);
	std::strcpy(str + 10, a_pathInfo.c_str());
	m_envp.push_back(str);
}

int	CGI::execute(std::string a_filePath)
{
	m_filePath = a_filePath;
	if ((m_status = scriptIsExecutable(a_filePath)))
		return (m_status);

	if ((m_status = setPath(a_filePath)))
		return (m_status);

	setArgv(a_filePath);
	setEnvp();

	m_status = run();
	deleteData();

	return (m_status);
}

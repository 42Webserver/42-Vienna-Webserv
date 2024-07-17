#include "CGI.hpp"

CGI::CGI(t_config config, Request& request) : m_path(NULL), m_config(config), m_request(request), m_pid(0), m_outputPipe(-1), m_status(0) {}

CGI::CGI(const CGI &other) : m_request(other.m_request)
{
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

CGI &CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		deleteData();

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
}

int	CGI::scriptIsExecutable(const std::string& a_filePath) const
{
	return (!access(a_filePath.c_str(), X_OK) ? 0 : 500);
}

int CGI::setPath(const std::string& a_filePath)
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

	vars.push_back("REQUEST_METHOD=" + m_request.getValue("method"));
	vars.push_back("CONTENT_TYPE=" + m_request.getValue("Content-Type"));
	vars.push_back("CONTENT_LENGTH=" + m_request.getBody().length());
	vars.push_back(SERVER_PROTOCOL_STRING);
	if (m_config.at("upload").size() == 1)
		vars.push_back("UPLOAD="+ m_config.at("upload").at(0));

	size_t	i = 0;
	while (i < vars.size())
	{
		std::cout << "COPY STRING = '" << vars.at(i) << "' WITH SIZE = " << vars.at(i).length() << std::endl;
		char *str = new char[vars.at(i).length() + 1];

		std::strcpy(str, vars.at(i).c_str());
		m_envp.push_back(str);
		std::cout << "AFTER COPY = '" << str << "'" << std::endl;
		i++;
	}
}

int CGI::run()
{
	int cgi_input[2], cgi_output[2];

    if (pipe(cgi_input) == -1 || pipe(cgi_output) == -1)
	{
		std::cout << "Error while opening pipe\n";
		return (500);
	}

	pid_t pid = fork();
	//PROTECTION
	if (pid < 0)
	{
		std::cout << "Error while forking\n";
		return (500);
	}
 	if (pid == 0)
    {
        dup2(cgi_output[1], STDOUT_FILENO);
        dup2(cgi_input[0], STDIN_FILENO);

        close(cgi_output[0]);
        close(cgi_output[1]);
        close(cgi_input[0]);
        close(cgi_input[1]);
		m_envp.push_back(NULL);
		m_argv.push_back(NULL);
		if (chdir(m_filePath.substr(0, m_filePath.find_last_of('/')).c_str()) == -1)
			exit(500);
		if (execve(m_path, m_argv.data(), m_envp.data()) == -1)
            exit(500);
        exit(0);
    }
    else
    {
        close(cgi_output[1]);
        close(cgi_input[0]);
		write(cgi_input[1], m_request.getBody().c_str(), m_request.getBody().length());
        close(cgi_input[1]);
		m_pid = pid;
		m_outputPipe = cgi_output[0];
    }
	return (0);
}

void CGI::deleteData()
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

int CGI::readFromPipe()
{
	if (m_outputPipe == -1)
		return (0);

	ssize_t n = 0;
	char buffer[4096];
	int	status_code = 0;

	if (startTime.isOver(CGI_TIMEOUT_SECONDS))
	{
		kill(m_pid, SIGINT);
		m_status = 500;
	}

	if (waitpid(m_pid, &status_code, WNOHANG) == 0)
	{
		return (-1);
	}


	if (WIFEXITED(status_code) && WEXITSTATUS(status_code) != 0)
		return ((m_status = 500));

	while ((n = read(m_outputPipe, buffer, sizeof(buffer))) > 0)
	{
		std::cout << "OUTPUT PIPE = " << m_outputPipe << " | N = " << n << std::endl;
		m_responseBody.append(buffer, n);
	}
	close(m_outputPipe);
	m_outputPipe = -1;
    return (0);
}

const std::string& CGI::getResponseBody() const
{
	return (m_responseBody);
}

int CGI::getStatusCode() const
{
	return (m_status);
}

void CGI::setUrlQuery(const std::string &a_urlQuery)
{
	std::string envvar = "QUERY_STRING=";
	envvar.append(a_urlQuery);
	char* str = new char[envvar.length() + 1];
	std::strcpy(str, envvar.c_str());
	m_envp.push_back(str);
}

void CGI::setPathInfo(const std::string &a_pathInfo)
{
	char* str = new char[a_pathInfo.length() + PATH_INFO_STRING_LENGTH + 1];
	std::strcpy(str, PATH_INFO_STRING);
	std::strcpy(str + 10, a_pathInfo.c_str());
	m_envp.push_back(str);
}

int	CGI::execute(std::string a_filePath)
{
	m_filePath = a_filePath;
	std::cout << "Check exec\n";
	if ((m_status = scriptIsExecutable(a_filePath)))
	{
		std::cout << "err: " << m_status << '\n';
		return (m_status);
	}

	std::cout << "set path\n";
	if ((m_status = setPath(a_filePath)))
		return (m_status);

	std::cout << "set argv envp\n" << std::endl;
	setArgv(a_filePath);
	setEnvp();

	std::cout << "PATH: " << m_path << std::endl;
	std::cout << "ARGS: " << m_argv[0] << " | " << m_argv[1] << std::endl;
	std::cout << "ENVP: ";
	for (size_t i = 0; i < m_envp.size(); i++)
		std::cout << " | " << m_envp.at(i);
	

	m_status = run();
	deleteData();

    return (m_status);
}

#include "CGI.hpp"

CGI::CGI(t_config config, Request& request) : m_config(config), m_request(request), m_pid(0), m_outputPipe(-1), m_status(0) {}

CGI::~CGI() {}

int	CGI::scriptIsExecutable()
{
	std::string filepath;
	filepath.append(m_config["root"].at(0));
	filepath.append(m_request.getValue("uri"));

	return (!access(filepath.c_str(), X_OK) ? 0 : 502);
}

int CGI::setPath()
{
	// schabernack, was ist wenn zb /cgi-bin/test/script.py ???
	std::string filename = m_request.getValue("uri").substr(9);

	if (filename.find(".py") == filename.length() - 3)
	{
		std::string	python_path = "/usr/bin/python3";
		m_path = new char[python_path.length() + 1];
		strcpy(m_path, python_path.c_str());
		return (0);
	}
	std::cout << "CGI-Error: Invalid cgi-script extension." << std::endl;
	return (502);
}

void	CGI::setArgv()
{
	m_argv = new char* [3];

	m_argv[0] = m_path;

	std::string scriptPath;
	scriptPath.append(m_config["root"].at(0));
	scriptPath.append(m_request.getValue("uri"));

	m_argv[1] = new char[scriptPath.length() + 1];
	strcpy(m_argv[1], scriptPath.c_str());

	m_argv[2] = NULL;
}

void	CGI::setEnvp()
{
	std::vector<std::string>	vars;

	vars.push_back("REQUEST_METHOD=" + m_request.getValue("method"));
	vars.push_back("CONTENT_TYPE=" + m_request.getValue("Content-Type"));
	vars.push_back("CONTENT_LENGTH=" + m_request.getValue("Content-Length"));

	m_envp = new char* [4];

	size_t	i = 0;
	while (i < vars.size())
	{
			std::cout << &m_envp[i] << std::endl;
			std::cout << "COPY STRING = '" << vars.at(i) << "' WITH SIZE = " << vars.at(i).length() << std::endl;
			m_envp[i] = new char[vars.at(i).length() + 1];

			strcpy(m_envp[i], vars.at(i).c_str());
			std::cout << "AFTER COPY = '" << m_envp[i] << "'" << std::endl;
			i++;
	}
	m_envp[i] = NULL;
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

		if (execve(m_path, m_argv, m_envp) == -1)
        {
            std::cout << "Error while calling execve\n";
            exit(500);
        }
        exit(0);
    }
    else
    {
        close(cgi_output[1]);
        close(cgi_input[0]);
		write(cgi_input[1], m_request.getBody().c_str(), m_request.getContentLength());
        close(cgi_input[1]);
		m_pid = pid;
		m_outputPipe = cgi_output[0];
    }
	return (0);
}



void CGI::deleteData()
{
	delete[] (m_path);
	delete[] (m_argv[1]);
	delete[] (m_argv);

	for (size_t	i = 0; m_envp[i] != NULL; ++i)
	{
		std::cout << &m_envp[i] << std::endl;
		std::cout << "DELETE = " << m_envp[i] << std::endl;
		std::cout << "i = " << i << std::endl;
		delete[] (m_envp[i]);
	}

	delete[] (m_envp);
}

int CGI::readFromPipe()
{
	if (m_outputPipe == -1)
		return (0);

	ssize_t n = 0;
	char buffer[4096];

	int	status_code = 0;
	if (waitpid(m_pid, &status_code, WNOHANG) == 0)
	{
		std::cout << "WAIT FOR CHILD PROCESS!" << std::endl;
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

int	CGI::execute()
{
	int	status_code = 0;

	if ((status_code = scriptIsExecutable()))
		return (status_code);

	if ((status_code = setPath()))
		return (status_code);

	setArgv();
	setEnvp();

	std::cout << "PATH: " << m_path << std::endl;
	std::cout << "ARGS: " << m_argv[0] << " | " << m_argv[1] << std::endl;
	std::cout << "ENVP: " << m_envp[0] << " | " << m_envp[1] << " | " << m_envp[2] << std::endl;

	status_code = run();
	deleteData();

    return (status_code);
}

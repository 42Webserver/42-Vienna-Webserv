#include "CGI.hpp"

CGI::CGI() {}

CGI::CGI(t_config config, Request request) : m_config(config), m_request(request) {}

CGI::CGI(const CGI &other)
{
	*this = other;
}

CGI &CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		*this = other;
	}
	return (*this);
}

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

	if (filename.find_last_of(".py") == filename.length() - 3)
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

	std::stringstream content_length;
	content_length << m_request.getBody().length();

	vars.push_back("CONTENT_LENGTH=" + content_length.str());
	vars.push_back("CONTENT_TYPE=" + m_request.getValue("Content-Type"));

	m_envp = new char* [3];

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





	// std::cout << "filename: " << filename << '\n';
	// std::cout << "location: " << m_config["name"].at(0) << '\n';
	// executeCGI(filename);
    return (status_code);
}

#include "CGI.hpp"

CGI::CGI() {}

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

void CGI::getExecutorPath(const std::string filename)
{
	if (filename.find_last_of(".py") == filename.length() - 3)
		m_executorPath = "/usr/bin/python3";
	else
		std::cout << "Error: Invalid cgi-script extension. (502 - Bad Gateway?)" << std::endl;
}

void CGI::execute(const std::string filename)
{
	//checkScriptFile();
	// check
		// file is openable / executable and has valid extension
	getExecutorPath(filename);
	//initArgs();
	//initEnv();





	std::cout << "filename: " << filename << '\n';
	// std::cout << "location: " << m_config["name"].at(0) << '\n';

	std::string filepath;



	// filepath.append(m_config["root"].at(0));
	// filepath.append(m_request.getValue("uri"));

    struct stat sb;
    if (stat(filepath.c_str(), &sb) == 0)
    {
        if (!S_ISREG(sb.st_mode))
		{
			std::cout << "not a file.\n";
			return;
		}
		if (access (filepath.c_str(), X_OK))
		{
			std::cout << "file not executable.\n";
			return;
		}
		std::cout << "file ok\n";
    }
	// executeCGI(filename);
    return;
}

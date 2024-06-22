#include "Response.hpp"

std::map<std::string, std::string>	Response::s_status_codes;
std::map<std::string, std::string>	Response::s_content_type;

Response::Response() {}

Response::Response(const Request& a_request, const t_config& a_config) : m_request(a_request), m_config(a_config)
{
	// std::cout << "HEEEEREEEEEEEEEEEEEE: " << m_request.getValue("method") << " " << m_request.getValue("uri") << '\n';
	// 		for (std::map<std::string, std::vector<std::string> > ::iterator it = m_config.begin(); it != m_config.end(); ++it)
	// 		{
	// 			std::cout << "		Key: " << it->first << " | value: ";
	// 			for (size_t j = 0; j < it->second.size(); j++)
	// 			{
	// 				std::cout << it->second.at(j) << ", ";
	// 			}
	// 			std::cout<< std::endl;
	// 		}
/* 			std::cout << "STATUS CODE =";
			for (std::map<std::string, std::string>::iterator it = s_status_codes.begin(); it != s_status_codes.end(); ++it)
			{
				std::cout << it->first + ": " + it->second + "\n";
			} */

}

Response::Response(const Response &other)
{
	*this = other;
}

Response &Response::operator=(const Response &other)
{
	if (this != &other)
	{
		m_request = other.m_request;
		m_responseHeader = other.m_responseHeader;
		m_responseBody = other.m_responseBody;
		m_config = other.m_config;
	}
	return (*this);
}

Response::~Response()
{
}

bool Response::getBody(std::string const &filename)
{
	if (!m_responseBody.empty())
		return true;
    std::ifstream input_file(filename.c_str());
    std::stringstream body;
	//std::cout << "Filename = " << filename << std::endl;

    if (!input_file.is_open() || !input_file.good())
    {
        std::cerr << "Error: open error file" << '\n';
        return (false);
	}
	body << input_file.rdbuf();
    m_responseBody.append(body.str());
    m_responseBody.append("\r\n");
	input_file.close();
    return (true);
}

void Response::initStatusCodes()
{
	s_status_codes["200"] = "OK";
	s_status_codes["301"] = "Moved Permanently";
	s_status_codes["302"] = "Found";
    s_status_codes["400"] = "Bad Request";
	s_status_codes["403"] = "Forbidden";
    s_status_codes["404"] = "Not Found";
    s_status_codes["405"] = "Method Not Allowed";
    s_status_codes["414"] = "URI Too Long";
    s_status_codes["500"] = "Internal Server Error";
    s_status_codes["505"] = "HTTP Version not supported";

	s_status_codes["0"] = "LANDING PAGE!";
}

void Response::initContentType()
{
	s_content_type["htm"] = "text/html";
	s_content_type["html"] = "text/html";
	s_content_type["shtml"] = "text/html";
	s_content_type["css"] = "text/css";
	s_content_type["xml"] = "text/xml";
	s_content_type["gif"] = "image/gif";
	s_content_type["jpeg"] = "image/jpeg";
	s_content_type["jpg"] = "image/jpeg";
	s_content_type["js"] = "application/javascript";
	s_content_type["ico"] = "image/x-icon";
	s_content_type["png"] = "image/png";
	s_content_type["webp"] = "image/webp";

}

const std::string Response::getResponse() const
{
	return (m_responseHeader + m_responseBody);
}

void Response::setValidMsg(const std::string &filepath)
{
	std::cout << "file	path	 = " << filepath << std::endl;
	if (!getBody(filepath))
		setErrorMsg(404);
	else
		getResponseHeader("200", "", getFileType(filepath));

}

std::string Response::getFileType(const std::string &filepath)
{
	size_t pos;
	std::string print;
	if ((pos = filepath.find_last_of(".")) != std::string::npos)
	{
		if (pos < filepath.length())
			return (filepath.substr(pos + 1, filepath.length()));
	}
	else if (filepath.at(filepath.length() - 1) == '/')
    	return ("html");
	return ("NOTHING");
}

void Response::setErrorMsg(const int &a_status_code)
{
	std::ostringstream convert;

	convert << a_status_code;
	std::map<std::string, std::vector<std::string> >::iterator found = m_config.find(convert.str());
	if (found == m_config.end())
	{
		setDefaultErrorMsg(convert.str());
		getResponseHeader(convert.str(), "", "html");
	}
	else
	{
		if (found->second.size() == 1)
			getResponseHeader("302", found->second.at(0), "html");
		else
			getResponseHeader("302", "", getFileType(found->second.at(1)));
	}
}

void Response::setDefaultErrorMsg(const std::string &a_status_code)
{
	m_responseBody.append("<!DOCTYPE html><html><title>");
	m_responseBody.append(s_status_codes[a_status_code]);
	m_responseBody.append("</title><h1>");
	m_responseBody.append(a_status_code + " " + s_status_codes[a_status_code]);
	m_responseBody.append("</h1></html>\r\n");
}

bool Response::checkAllowedMethod()
{
	for (size_t i = 0; i < m_config["allowed_methods"].size(); i++)
	{
		if (m_config["allowed_methods"].at(i) == m_request.getValue("method"))
			return (true);
	}
	return (false);
}

int Response::checkHeaderline()
{
	if (m_request.getValue("http_version") != "HTTP/1.1")
		return (505);
	if (!checkAllowedMethod())
		return (405);
	return (0);
}

int Response::getValidFilePath(std::string &a_filepath)
{
    int    ret = isValidFile(a_filepath);
    if (ret == 403)
    {
        std::string temp;
        if (m_config.at("index").size())
        {
            temp = a_filepath + m_config.at("index").at(0);
            ret = getValidFilePath(temp);
            a_filepath = temp;
            return (ret);
        }
        if (m_config.at("autoindex").size())
        {
            if (m_config.at("autoindex").at(0) == "on")
            {
				createAutoIndex(a_filepath);
                return (0);
            }
        }
    }
    return (ret);
}

// @brief
/// @param a_filepath
/// @return Returns 0 for file.
///            Returns 403 for dir
///         Returns 301 for dir when searching for file
///         Returns 404 for no dir or file

int Response::isValidFile(std::string &a_filepath)
{
    struct stat sb;
    if (stat(a_filepath.c_str(), &sb) == 0)
    {
        if (S_ISREG(sb.st_mode))
            return (0);
        if (S_ISDIR(sb.st_mode))
        {
            if (a_filepath.size() > 0 && a_filepath.at(a_filepath.size() - 1) != '/')
            {
                a_filepath.push_back('/');
                return (301);
            }
            return (403);
        }
    }
    return (404);
}

bool	Response::checkReturnResponse()
{
	if (m_config["return"].size())
	{
			//setErrorMsg(); send error page! ! ! !
		if (m_config["return"].size() == 2)
			getResponseHeader(m_config.at("return").at(0), m_config.at("return").at(1), "html");
		else
		{
			setDefaultErrorMsg(m_config.at("return").at(0));
			getResponseHeader(m_config.at("return").at(0), "", "html");
		}
		return (true);
	}
	return (false);
}

void Response::executeCGI(const std::string filename)
{
	(void)filename;

	int cgi_input[2], cgi_output[2];

    if (pipe(cgi_input) == -1 || pipe(cgi_output) == -1)
	{
		std::cout << "Error while opening pipe\n";
		return;
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		std::cout << "Error while forking\n";
		return;
	}
 	if (pid == 0)
    {
        // Child process
        dup2(cgi_output[1], STDOUT_FILENO);
        dup2(cgi_input[0], STDIN_FILENO);

        close(cgi_output[0]);
        close(cgi_output[1]);
        close(cgi_input[0]);
        close(cgi_input[1]);

		// std::ostringstream os;
		// os << m_request.getBody().length();
        // std::string content_length = "CONTENT_LENGTH=" + os.str();
        // std::string content_type = "CONTENT_TYPE=" + m_request.getValue("Content-Type");
        // char* envp[] = {
        //     const_cast<char*>(content_length.c_str()),
        //     const_cast<char*>(content_type.c_str()),
        //     NULL
        // };





		// char* envp[] = {NULL};
        // char python_path[] = "/usr/bin/python3";
        // char script_path[] = "/home/fheid/42-Vienna-Webserv/www/cgi-bin/file-upload.py";
        // char* args[] = {python_path, script_path, NULL};

		const char pythonPath[] = "/usr/bin/python3";

		std::string scriptPath;

		scriptPath.append(m_config["root"].at(0));
		scriptPath.append(m_request.getValue("uri"));


        // const char scriptPath[] = "/home/fheid/42-Vienna-Webserv/www/cgi-bin/welcome.py";

		// Prepare the arguments (the first argument should be the script name)
		std::vector<const char*> args;
		args.push_back(pythonPath);
		args.push_back(const_cast<char*>(scriptPath.c_str()));
		args.push_back(NULL);

		// Set up environment variables required for the CGI script
		std::vector<const char*> envp;
		envp.push_back("REQUEST_METHOD=POST");
		envp.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");

		std::stringstream content_length;

		content_length << "CONTENT_LENGTH=";
		content_length << m_request.getBody().length();

		envp.push_back(content_length.str().c_str());
		envp.push_back(NULL);




        // if (execve(python_path, args, envp) == -1)
		if (execve(pythonPath, const_cast<char* const*>(args.data()), const_cast<char* const*>(envp.data())) == -1)
        {
            std::cout << "Error while calling execve\n";
            exit(1);
        }
        exit(0);
    }
    else
    {
        // Parent process
        close(cgi_output[1]);
        close(cgi_input[0]);

        // Send the POST data to the CGI script
        // write(cgi_input[1], m_request.getBody().c_str(), m_request.getBody().length());
		// Data to be passed to the script through stdin (CGI input)
		// const char* inputData = "first_name=John&last_name=Doe";

		std::cout << "HIER: " << m_request.getBody() << std::endl;

		std::string inputDataString = m_request.getBody();
		write(cgi_input[1], m_request.getBody().c_str(), m_request.getBody().length());
        close(cgi_input[1]);

        // Read the output from the CGI script
        m_responseHeader.clear();
        m_responseHeader = "HTTP/1.1 200 OK\r\n";
        m_responseHeader.append("Content-Type: text/html\r\n\r\n");

        char buffer[1024];
        ssize_t n;
        while ((n = read(cgi_output[0], buffer, sizeof(buffer))) > 0)
        {
            m_responseBody.append(buffer, n);
        }
        close(cgi_output[0]);
        waitpid(pid, NULL, 0);
    }
}

void Response::handleCGI(const std::string filename)
{
	std::cout << "filename: " << filename << '\n';

	// std::map<std::string, std::vector<std::string> >::iterator it;

	// for (it = m_config.begin(); it != m_config.end(); ++it)
	// 	std::cout << (*it).first << " | " << ((*it).second.size() ? (*it).second.at(0) : "XXX") << std::endl;

	std::cout << "location: " << m_config["name"].at(0) << '\n';

	std::string filepath;

	filepath.append(m_config["root"].at(0));
	filepath.append(m_request.getValue("uri"));

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
	executeCGI(filename);
    return;
}

void Response::createResponseMsg()
{
	int error_code;

	if (!m_request.getIsValid())
	{
		setErrorMsg(400);
		return ;
	}
	if ((error_code = checkHeaderline()))
	{
		setErrorMsg(error_code);
		return ;
	}
	if (m_request.getValue("method") == "GET")
	{
		if (checkReturnResponse())
			return ;

		std::string filepath;

		filepath.append(m_config["root"].at(0));
		filepath.append(m_request.getValue("uri"));
	 	if ((error_code = getValidFilePath(filepath)))
		{
			if (error_code == 301)
			{
				filepath.erase(0, m_config.at("root").at(0).length());
				getResponseHeader("301", filepath, "html");
				return ;
			}
			setErrorMsg(error_code);
		}
		else
			setValidMsg(filepath);
	}
	else if (m_request.getValue("method") == "POST")
	{
		if (checkReturnResponse())
			return ;

		std::cout << "\n\n\nPOST-REQUEST\n\n" << std::endl;
		std::cout << "> uri: " << m_request.getValue("uri") << '\n';

		if (m_request.getValue("uri").find_first_of("/cgi/bin/") == 0)
		{
			std::cout << "CGI!!!" << std::endl;
			if (m_request.getValue("uri").length() > 9)
				handleCGI(m_request.getValue("uri").substr(9));
		}
	}
}

void Response::clearBody()
{
	m_responseBody.clear();
}

void Response::createAutoIndex(std::string &a_path)
{
	DIR* dir = opendir(a_path.c_str());
	if (dir == NULL)
		return ;
	std::string uri = a_path;
	uri.erase(0,  m_config.at("root").at(0).length());
	m_responseBody.append("<!DOCTYPE html><body><h1>Index of " + uri + "</h1><hr><div style=\"display: flex; flex-direction: column; justify-items: center; align-items: flex-begin;\">");
	struct dirent* de = readdir(dir);
	std::size_t start = m_responseBody.length();
	std::size_t dirs = 0;
	std::size_t files = 0;
	while ((de = readdir(dir)) != NULL)
	{
		std::string temp;
		if (de->d_type == DT_DIR) {
			temp = "<a href=\"" + uri + de->d_name  + "/\">" + de->d_name + "/</a>";
			m_responseBody.insert(start + dirs, temp);
			dirs += temp.length();
		} else {
			temp = "<a href=\"" + uri + de->d_name  + "\">" + de->d_name + "</a>";
			m_responseBody.insert(start + dirs + files, temp);
			files += temp.length();
		}
	}
	m_responseBody.append("</div><hr></body>\r\n");
	closedir(dir);
}

//////////////////////+++++Response Header+++++++++++//////////////////////

void Response::getResponseHeader(const std::string &a_status_code, const std::string &a_redirLoc, const std::string &a_content_type)
{
	std::string response_header;
	addStatusLine(a_status_code, response_header);
	addServerName(response_header);
	addDateAndTime(response_header);
	//Content-type!
	addContentType(response_header, a_content_type);
	addContentLength(response_header);
	//Connection: keep-alive!
	addRedirection(response_header, a_redirLoc);
	response_header.append("\r\n");
	m_responseHeader += response_header;
}

void	Response::addStatusLine(const std::string &a_status_code, std::string& a_response_header)
{
	a_response_header.append(m_request.getValue("http_version"));
	a_response_header += ' ';
	a_response_header.append(a_status_code);
	a_response_header += ' ';
	a_response_header.append(s_status_codes[a_status_code]);
	a_response_header.append("\r\n");
}

void Response::addDateAndTime(std::string &a_response_header)
{
	std::time_t t = std::time(NULL);
    std::tm* now = std::localtime(&t);

	char buffer[32];
	a_response_header.append("Date: ");
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S CEST", now);

	a_response_header.append(buffer);
	a_response_header.append("\r\n");
}

void Response::addContentLength(std::string &a_response_header)
{
	std::ostringstream convert;

	convert << m_responseBody.length();
	a_response_header.append("Content-Length: ");
	a_response_header.append(convert.str());
	a_response_header.append("\r\n");
}

void Response::addRedirection(std::string &a_response_header, const std::string &redLoc)
{
	a_response_header.append("Location: ");
	a_response_header.append(redLoc);
	a_response_header.append("\r\n");
}

void Response::addServerName(std::string &a_response_header)
{
	a_response_header.append("Server: ");
	a_response_header.append(SERVERNAME);
	a_response_header.append("\r\n");
}

void Response::addContentType(std::string &a_response_header, const std::string &a_content_type)
{
	a_response_header.append("Content-Type: ");
	if (s_content_type.find(a_content_type) != s_content_type.end())
		a_response_header.append(s_content_type[a_content_type]);
	else
		a_response_header.append("text/plain");
	a_response_header.append("\r\n");
}


// check is Request is valid? => if (false ) ? badRequest : weiter
// check httpVersion!
// check check and set uri!
// check method
// get Method
//  check if return => if (true) ? return statuscode and redirection with key Location:
//  check valid root
//	check autoindex if (true) ? root => displayen directory tree : send index
//  send index: file

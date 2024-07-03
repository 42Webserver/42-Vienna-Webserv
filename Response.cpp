#include "Response.hpp"

std::map<std::string, std::string>	Response::s_status_codes;
std::map<std::string, std::string>	Response::s_content_type;

Response::Response(Request &a_request) : m_request(a_request),  m_eventFlags(0), m_cgi(NULL) {}


Response::Response(Request &a_request, const t_config &a_config) : m_request(a_request), m_config(a_config), m_eventFlags(0), m_cgi(NULL)
{
	std::cout << "ICH SETZE CGI AUF NULL" << std::endl;
/* 			std::cout << "STATUS CODE =";
			for (std::map<std::string, std::string>::iterator it = s_status_codes.begin(); it != s_status_codes.end(); ++it)
			{
				std::cout << it->first + ": " + it->second + "\n";
			} */

}

Response::Response(Request &a_request, const Response &other) : m_responseHeader(other.m_responseHeader), m_responseBody(other.m_responseBody), m_request(a_request), m_config(other.m_config), m_eventFlags(other.m_eventFlags), m_cgi(other.m_cgi){}

Response::Response(const Response &other) : m_responseHeader(other.m_responseHeader), m_responseBody(other.m_responseBody), m_request(other.m_request), m_config(other.m_config), m_eventFlags(other.m_eventFlags), m_cgi(other.m_cgi) {}

Response &Response::operator=(const Response &other)
{
	if (this != &other)
	{
		m_request = other.m_request;
		m_responseHeader = other.m_responseHeader;
		m_responseBody = other.m_responseBody;
		m_config = other.m_config;
		m_eventFlags = other.m_eventFlags;
		m_cgi = other.m_cgi;
	}
	return (*this);
}

Response::~Response()
{
	delete m_cgi;
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
	s_status_codes["413"] = "Content Too Large";
	s_status_codes["414"] = "URI Too Long";
	s_status_codes["431"] = "Request Header Fields Too Large";
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
	if (!getBody(filepath))
		setErrorMsg(404);
	else
		getResponseHeader("200", "", getFileType(filepath));
}

std::string Response::getFileType(const std::string &filepath)
{
	if (filepath.empty())
		return ("html");
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
		{
			m_eventFlags |= REDIRECTION;
			getResponseHeader("302", found->second.at(0), getFileType(found->second.at(0)));
		}
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
	if (ret == 4031)
	{
		if (m_config.at("index").size())
		{
			std::string temp = a_filepath + m_config.at("index").at(0);
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
		return (403);
	}
	return (ret);
}

// @brief
/// @param a_filepath
/// @return Returns 0 for file.
///			Returns 403 for no perms;
///         Returns 4031 for dir
///         Returns 301 for dir when searching for file
///         Returns 404 for no dir or file

int Response::isValidFile(std::string &a_filepath)
{
	struct stat sb;
	if (stat(a_filepath.c_str(), &sb) == 0)
	{
		if ((sb.st_mode & S_IRUSR) == 0)
			return (403);
		if (S_ISREG(sb.st_mode))
			return (0);
		if (S_ISDIR(sb.st_mode))
		{
			if (a_filepath.size() > 0 && a_filepath.at(a_filepath.size() - 1) != '/')
			{
				a_filepath.push_back('/');
				return (301);
			}
			return (4031);
		}
	}
	return (404);
}

int	Response::isReturnResponse()
{
	if (m_config["return"].size())
	{
		m_eventFlags |= REDIRECTION;
		if (m_config["return"].size() == 2)
			m_eventFlags |= REDIR_LOCATION;
		return (static_cast<int>(strtol(m_config.at("return").at(0).c_str(), NULL, 10)));
	}
	return (0);
}

int	Response::isValidRequestHeader()
{
	int error_code;
	if ((error_code = m_request.getIsValid()) == 0)
		return (checkHeaderline());
	return (error_code);
}

bool Response::isCgiResponse()
{
	return (m_cgi != NULL);
}

bool Response::createResponseMsg()
{
	int error_code;
	std::string filepath;

	if (isCgiResponse())
	{
		if (isCgiReady())
		{
			if (m_cgi->getStatusCode())
				setErrorMsg(m_cgi->getStatusCode());
			else
			{
				m_responseBody = m_cgi->getResponseBody();
				// std::cout << "CGI Rsponese: " << m_responseBody << '\n';
				getResponseHeader("200", "", "html");
			}
			return (true);
		}
		return (false);
	}
	else if (!(error_code = isValidRequestHeader()))
	{
		if (!(error_code = isReturnResponse()))
		{
			if (m_request.getValue("method") == "GET")
			{

				filepath.append(m_config["root"].at(0));
				filepath.append(m_request.getValue("uri"));
				if ((error_code = getValidFilePath(filepath)))
				{
					if (error_code == 301)
					{
						m_eventFlags |= REDIRECTION | REDIR_LOCATION;
						m_config["return"].push_back("301");
						m_config["return"].push_back(filepath.erase(0, m_config.at("root").at(0).length()));
					}
				}
			}
			else if (m_request.getValue("method") == "POST")
			{
				// if (isReturnResponse())
				// 	return ;
				LOG("POST-REQUEST");
				if (m_request.getValue("uri").find_first_of("/cgi/bin/") == 0)
				{
					LOG("CGI")
					m_cgi =	new CGI(m_config, m_request);
					int ret = m_cgi->execute();

					//HIER MUSS NOCH EINMAL DAS MIT READ FROM PIPE REINGEMACHT WERDEN UND DIE ISCGIREADY FUNCTION GECALLT WERDEN!!!!!!
					//m_responseBody = m_cgi->getResponseBody();
					error_code = ret;
					if (error_code == 0)
						return (false);
				}
			}
		}
	}
	if (error_code)
		setErrorMsg(error_code);
	else
		setValidMsg(filepath);
	return (true);
}


// is GET METHOD => check if uri if it is a PATH_INFO, then if there is an query string, safe it in envp!
//call with execve the file! 
// if method post and is uri is path info => pipe body and execve the python script 
//Everything what is inside in PATH_INFO has to be a script so it will be executed!!
//UPLOAD value will be stored in envp that the script knows where to upload! 


void Response::clearBody()
{
	m_responseBody.clear();
}

bool	Response::isCgiReady()
{
	return (m_cgi->readFromPipe() != -1);
}

std::size_t Response::getMaxBodySize(void) const
{
	t_config::const_iterator found = m_config.find("client_max_body_size");
	if (found != m_config.end())
	{
		if (found->second.size() != 0)
			return (std::strtol(found->second.at(0).c_str(), NULL, 10) * BYTE_TO_KB);
	}
	return std::size_t(0);
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
	m_responseHeader.clear();
	addStatusLine(a_status_code, response_header);
	addServerName(response_header);
	addDateAndTime(response_header);
	//Content-type!
	addContentType(response_header, a_content_type);
	addContentLength(response_header);
	//Connection: keep-alive!
	addConnection(a_status_code, response_header);
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

void Response::addConnection(const std::string& a_status_code, std::string &a_response_header)
{
	const std::string& requestConnection = m_request.getValue("Connection");
	if (a_status_code > "302")
		a_response_header.append("Connection: close\r\n");
	else if (!requestConnection.empty())
	{
		a_response_header.append("Connection: ");
		a_response_header.append(requestConnection);
		a_response_header.append("\r\n");
	}
}

void Response::addContentLength(std::string &a_response_header)
{
	std::ostringstream convert;

	convert << m_responseBody.length();
	a_response_header.append("Content-Length: ");
	a_response_header.append(convert.str());
	a_response_header.append("\r\n");
}

void Response::addRedirection(std::string &a_response_header, const std::string &a_redLoc)
{
	if (m_eventFlags & REDIRECTION)
	{
		a_response_header.append("Location: ");
		if (m_eventFlags & REDIR_LOCATION)
			a_response_header.append(m_config.at("return").at(1));
		else
			a_response_header.append(a_redLoc);
		a_response_header.append("\r\n");

	}
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

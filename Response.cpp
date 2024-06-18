#include "Response.hpp"

std::map<std::string, std::string>	Response::s_status_codes;
std::map<std::string, std::string>	Response::s_content_type;

Response::Response() {}

Response::Response(const Request& a_request, const t_config& a_config) : m_request(a_request), m_config(a_config)
{

// /* 			for (std::map<std::string, std::vector<std::string> > ::iterator it = m_config.begin(); it != m_config.end(); ++it)
// 			{
// 				std::cout << "		Key: " << it->first << " | value: ";
// 				for (size_t j = 0; j < it->second.size(); j++)
// 				{
// 					std::cout << it->second.at(j) << ", ";
// 				}
// 				std::cout<< std::endl;
// 			} */
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
	s_content_type["txt"] = "text/plain";
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
		getResponseHeader("200", "");
	
}

void Response::setErrorMsg(const int &a_status_code)
{
	std::ostringstream convert; 

	convert << a_status_code;
	std::map<std::string, std::vector<std::string> >::iterator found = m_config.find(convert.str());
	if (found == m_config.end())
	{
		setDefaultErrorMsg(convert.str());
		getResponseHeader(convert.str(), "");
	}	
	else 
	{
		if (found->second.size() == 1)
			getResponseHeader("302", found->second.at(0));
		else
			getResponseHeader("302", "");
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
                std::cout << "AUTOINDEX HIER HIN BITTE FLO!" << '\n';
				createAutoIndex(a_filepath);
                return (0);
            }
        }
    }
    return (ret);
}

/// @brief 
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
		if (m_config["return"].size() == 2)
			getResponseHeader(m_config.at("return").at(0), m_config.at("return").at(1));
		else 
		{
			setDefaultErrorMsg(m_config.at("return").at(0));
			getResponseHeader(m_config.at("return").at(0), "");
		}	
		return (true);
	}
	return (false);
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
			if (error_code == 301)
			{
				filepath.erase(0, m_config.at("root").at(0).length());
				getResponseHeader("301", filepath);
				return ;
			}
			std::cout << "ALAAAARM! = " << error_code << std::endl;
			setErrorMsg(error_code);	
		else 
			setValidMsg(filepath);
		std::cout << "Filepath = " << filepath << std::endl;
	}
}

//////////////////////+++++Response Header+++++++++++//////////////////////

void Response::getResponseHeader(const std::string &a_status_code, const std::string &a_redirLoc)
{
	std::string response_header;
	addStatusLine(a_status_code, response_header);
	addServerName(response_header);
	addDateAndTime(response_header);
	//Content-type!
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
	std::cout << m_responseBody;
	closedir(dir);
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

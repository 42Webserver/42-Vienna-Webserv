#include "Response.hpp"

std::map<std::string, std::string>	Response::s_status_codes;
std::map<std::string, std::string>	Response::s_content_type;

Response::Response(Request &a_request) : m_request(a_request),  m_eventFlags(0), m_cgi(NULL) {}

Response::Response(Request &a_request, const t_config &a_config) : m_request(a_request), m_config(a_config), m_eventFlags(0), m_cgi(NULL) {}

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

Response::~Response() {}

bool Response::getBody(std::string const &filename)
{
	if (!m_responseBody.empty())
		return true;
	std::ifstream input_file(filename.c_str());
	std::stringstream body;
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
	return (headerMapToString() + m_responseBody);
}

void Response::setValidMsg(const std::string &filepath)
{
	if (m_cgi)
		return ;
	if (!getBody(filepath))
		setErrorMsg(404);
	else
		getResponseHeader("200", "", getFileType(filepath));
}

std::string Response::getFileType(const FilePath &filepath)
{
	std::string ext = filepath.extension();
	return (ext.empty()? "html" : ext.erase(0, 1));
}

void Response::setErrorMsg(const int &a_status_code)
{
	if (a_status_code == -1)
	{
		getResponseHeader("200", "", "");
		return;
	}
	else if (m_cgi && !m_responseBody.empty())
	{
		getResponseHeader("500", "", "html");		
		return ;
	}
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

bool Response::checkAllowedMethod(const std::string& a_methodList) const
{
	t_config::const_iterator methods = m_config.find(a_methodList);
	if (methods == m_config.end() || methods->second.size() == 0)
		return (false);
	for (size_t i = 0; i < methods->second.size(); i++)
	{
		if (methods->second.at(i) == m_request.getValue("method"))
			return (true);
	}
	return (false);
}

int Response::checkHeaderline()
{
	if (m_request.getValue("http_version") != "HTTP/1.1")
		return (505);
	if (!checkAllowedMethod("allowed_methods"))
		return (405);
	return (0);
}

int Response::getValidFilePath(std::string &a_filepath, std::string& a_pathInfo)
{
	FilePath path = a_filepath;

	while (!path.exists())
		path.remove_last();
	a_filepath.erase(0, path.str().length());
	a_pathInfo = a_filepath;
	a_filepath = path.str();
	if (!path.isRead())
		return (403);
	if (path.isDir())
	{
		if (path.str().length() && *(path.str().end() - 1) != '/' && a_pathInfo.empty())
		{
			a_filepath = path.str();
			a_filepath.push_back('/');
			return (301);
		}
		else if (m_config.at("index").size())
		{
			path.append(m_config.at("index").at(0));
			a_filepath = path.str();
			if (!path.exists())
				return (404);
			return (0);
		}
		else if (!a_pathInfo.empty())
			return (404);
		else if (m_config.at("autoindex").size())
		{
			if (m_config.at("autoindex").at(0) == "on")
			{
				createAutoIndex(path.str());
				return (0);
			}
		}
		return (403);
	}
	return (0);
}

/// @brief Takes the a_uri and seperates it into filepath and uriQuery
/// @param a_uri the request uri
/// @param a_query the string where we write the query into
/// @return returns the path to the file (config root + uri) 
std::string Response::decodeUri(const std::string &a_uri, std::string &a_query)
{
	std::string uriPath = m_config.at("root").at(0);
	std::size_t	encPos = a_uri.find_first_of('?');
	uriPath.append(a_uri, 0, encPos);
	if (encPos != std::string::npos && encPos + 1 < a_uri.length())
		a_query.append(a_uri, encPos + 1);
	return (uriPath);
}

int	Response::isReturnResponse()
{
	if (m_config["return"].size())
	{
		m_responseBody.clear();
		m_eventFlags |= REDIRECTION;
		if (m_config["return"].size() == 2)
			m_eventFlags |= REDIR_LOCATION;
		return (static_cast<int>(std::strtol(m_config.at("return").at(0).c_str(), NULL, 10)));
	}
	return (0);
}

int Response::deleteRequest(const FilePath& a_filePath)
{
	if (a_filePath.exists())
	{
		if (a_filePath.isFile())
		{
			if (std::remove(a_filePath.c_str()))
				return (404);
			m_responseBody.append("<html><body><h1>Delete file successfull</h1></body></html>\r\n");
			return (0);		
		}
		return (403);
	}
	return (404);
}

void Response::modifyUri()
{
	if (m_config.find("name") != m_config.end() && m_config.at("name").at(0) != "/")
	{
		std::string newUri;
		
		if (m_request.getValue("uri").length() >= m_config.at("name").at(0).length())
			newUri = m_request.getValue("uri").substr(m_config.at("name").at(0).length());
		if (newUri.empty())
			newUri.append("/");
		m_request.setUri(newUri);
	}

}

void Response::insertCgiResponse()
{
	std::size_t sepPos = m_responseBody.find("\r\n\r\n");
	std::stringstream headStuff;
	if (sepPos != std::string::npos)
	{
		headStuff << m_responseBody.substr(0, sepPos + 4);
		m_responseBody.erase(0, sepPos + 4);
	}
	getResponseHeader("200", "", "html");
	std::string line;
	while (std::getline(headStuff, line, '\n'))
	{
		std::size_t colPos = line.find_first_of(':');
		if (colPos != std::string::npos)
		{
			std::size_t valBegin = line.find_first_not_of(" \t:", colPos);
			m_responseHeader[line.substr(0, colPos)] = line.substr(valBegin == std::string::npos ? colPos : valBegin);
		}
	}
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
	return (m_cgi);
}

bool Response::createResponseMsg()
{
	int error_code = 0;
	std::string filepath;
	std::string urlQuery;
	std::string pathInfo;

	if (isCgiResponse())
	{
		if (isCgiReady())
		{
			m_responseBody = m_cgi->getResponseBody();
			if (m_cgi->getStatusCode())
				error_code = m_cgi->getStatusCode();
			else
			{
				insertCgiResponse();
				//return (true);
			}
		}
		else
			return (false);
	}
	else if (!(error_code = isValidRequestHeader()))
	{
		modifyUri();
		filepath = decodeUri(m_request.getValue("uri"), urlQuery);
		if ((error_code = getValidFilePath(filepath, pathInfo)))
		{
			if (error_code == 301)
			{
				m_eventFlags |= REDIRECTION | REDIR_LOCATION;
				m_config["return"].push_back("301");
				filepath.erase(0, m_config.at("root").at(0).length());
				if (m_config.find("name") != m_config.end())
					filepath.insert(0, m_config.at("name").at(0));
				m_config["return"].push_back(filepath);
			}
		}
		else if (isCgiFile(filepath))
		{
			LOG("CGI POST-REQUEST");
			m_cgi =	SharedPtr<CGI>(new CGI(m_config, m_request));
			m_cgi->setUrlQuery(urlQuery);
			m_cgi->setPathInfo(pathInfo);
			error_code = m_cgi->execute(filepath);
			if (error_code == 0)
				return (false);
		}
		else if (!pathInfo.empty())
			error_code = 404;
		else if (m_request.getValue("method") == "POST")
			error_code = -1;
		else if(m_request.getValue("method") == "DELETE")
			error_code = deleteRequest(filepath);
		if (m_eventFlags & CGI_METH_DENY)
				error_code = 403;
	}
	int ret = isReturnResponse();
	if (ret)
		error_code = ret;
	if (error_code)
		setErrorMsg(error_code);
	else
		setValidMsg(filepath);
	return (true);
}

void Response::clearBody()
{
	m_responseBody.clear();
}

bool	Response::isCgiReady()
{
	return (m_cgi->readFromPipe() != -1);
}

bool Response::isCgiFile(const FilePath &a_filePath)
{
	if (m_config.find("name") == m_config.end() || m_config.find("extension") == m_config.end())
		return (false);
	const std::vector<std::string>& extensions = m_config.at("extension");
	std::string fileExtention = a_filePath.extension();
	for (std::size_t i = 0; i < extensions.size(); i++)
	{
		if (fileExtention == extensions.at(i))
		{
			if (!checkAllowedMethod("cgi_methods"))
				return (m_eventFlags |= CGI_METH_DENY, false);
			return (true);
		}
	}
	return (false);
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

static bool operator<(dirent lhs, dirent rhs)
{
	return (lhs.d_type < rhs.d_type || std::strcmp(lhs.d_name, rhs.d_name) < 0);
}

void Response::createAutoIndex(const std::string &a_path)
{
	DIR* dir = opendir(a_path.c_str());
	if (dir == NULL)
		return ;
	std::string uri = a_path;
	uri.erase(0,  m_config.at("root").at(0).length());
	if (m_config.find("name") != m_config.end())
		uri.insert(0, m_config.at("name").at(0));
	m_responseBody.append("<!DOCTYPE html><body><h1>Index of " + uri
		+ "</h1><hr><div style=\"display: flex; flex-direction: column; justify-items: center; align-items: flex-begin;\">");
	struct dirent* de;
	std::vector<dirent> ents;
	while ((de = readdir(dir)) != NULL)
		ents.push_back(*de);
	std::sort(ents.begin(), ents.end());
	ents.erase(ents.begin());
	for (std::size_t i = 0; i < ents.size(); i++)
	{
		if (ents.at(i).d_type == DT_DIR){
			m_responseBody.append("<a href=\"" + uri + ents.at(i).d_name  + "/\">" + ents.at(i).d_name + "/</a>");

		} else if (ents.at(i).d_name[0] != '.') {
			m_responseBody.append("<a href=\"" + uri + ents.at(i).d_name  + "\">" + ents.at(i).d_name + "</a>");
		}
	}
	m_responseBody.append("</div><hr></body>\r\n");
	closedir(dir);
}

//////////////////////+++++Response Header+++++++++++//////////////////////

std::string Response::headerMapToString(void) const
{
	std::string headerStr;
	std::map<std::string, std::string>::const_iterator statusLine = m_responseHeader.find("HTTP/1.1");
	if (statusLine != m_responseHeader.end())
		headerStr += "HTTP/1.1 " + statusLine->second + "\r\n";
	for (std::map<std::string, std::string>::const_iterator i = m_responseHeader.begin(); i != m_responseHeader.end(); i++)
	{
		if (i == statusLine)
			continue;
		headerStr += i->first + ": " + i->second + "\r\n";
	}
	headerStr.append("\r\n");
	return (headerStr);	
}

void Response::getResponseHeader(const std::string &a_status_code, const std::string &a_redirLoc, const std::string &a_content_type)
{
	m_responseHeader.clear();
	m_responseHeader["HTTP/1.1"] = a_status_code + ' ' + s_status_codes[a_status_code];
	m_responseHeader["Server"] = SERVERNAME;
	addDateAndTime();
	//Content-type!
	addContentType(a_content_type);
	addContentLength();
	//Connection: keep-alive!
	addConnection(a_status_code);
	addRedirection(a_redirLoc);
}

void Response::addDateAndTime()
{
	std::time_t t = std::time(NULL);
	std::tm* now = std::localtime(&t);

	char buffer[32];
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S CEST", now);

	m_responseHeader["Date"] = buffer;
}

void Response::addConnection(const std::string& a_status_code)
{
	const std::string& requestConnection = m_request.getValue("Connection");
	if (a_status_code > "302")
		m_responseHeader["Connection"] = "close";
	else if (!requestConnection.empty())
		m_responseHeader["Connection"] = requestConnection;
}

void Response::addContentLength()
{
	std::ostringstream convert;

	convert << m_responseBody.length();
	m_responseHeader["Content-Length"] = convert.str();
}

void Response::addRedirection(const std::string &a_redLoc)
{
	if (m_eventFlags & REDIRECTION)
	{
		if (m_eventFlags & REDIR_LOCATION)
			m_responseHeader["Location"] = m_config.at("return").at(1);
		else
			m_responseHeader["Location"] = a_redLoc;
	}
}

void Response::addContentType(const std::string &a_content_type)
{
	if (s_content_type.find(a_content_type) != s_content_type.end())
		m_responseHeader["Content-Type"] = s_content_type[a_content_type];
	else
		m_responseHeader["Content-Type"] = "text/plain";
}

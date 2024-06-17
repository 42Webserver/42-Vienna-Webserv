#include "Response.hpp"

Response::Response() {}

Response::Response(const Request& a_request, const t_config& a_config) : m_request(a_request), m_config(a_config)
{

			for (std::map<std::string, std::vector<std::string> > ::iterator it = m_config.begin(); it != m_config.end(); ++it)
			{
				std::cout << "		Key: " << it->first << " | value: ";
				for (size_t j = 0; j < it->second.size(); j++)
				{
					std::cout << it->second.at(j) << ", ";
				}
				std::cout<< std::endl;
			}
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
		m_responseMsg = other.m_responseMsg;
		m_config = other.m_config;
	}
	return (*this);
}

Response::~Response()
{
}

//check is Request is valid? => if (false ) ? badRequest : weiter
//check httpVersion! 
//check check and set uri!
//check method
//get Method
// check if return => if (true) ? return statuscode and redirection with key Location:
// check valid root
//	check autoindex if (true) ? root => displayen directory tree : send index
// send index: file 

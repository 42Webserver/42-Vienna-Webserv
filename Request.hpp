#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>

class Request
{
public:

	Request(void);
	Request(const Request& other);
	// Request(std::string& head);
	Request& operator=(const Request& other);
	~Request();

	int					getClientSocket() const;
	const std::string&	getValue(const std::string& a_key);
	bool				getValue(const std::string& a_key, std::string& a_returnValue) const;
	void				setValue(const std::string& a_key, const std::string& a_val);
	std::string			getRequestHost() const;
	bool				getIsValid(void) const;
	size_t				getContentLength() const;
	void				addHead(const std::string& a_head);
	bool				headComplete(void);
	void				addBody(const std::string& a_body);
	bool				bodyComplete(void) const;
	bool				isReady(void);
	const std::string&	getHead();
	const std::string&	getBody();
	void				initMap();

private:

	void	getRequestLine(std::string& line);
	void	createKeyValuePair(std::string line);
	bool								m_isValid;
	bool								m_headComplete;
	std::string							m_head;
	std::string							m_body;
	std::map <std::string, std::string>	m_requestHeader;
};

#endif // !REQUEST_HPP

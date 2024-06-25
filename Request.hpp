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

#include "Debug.h"

#define MAX_HEAD_SIZE 8192
#define MAX_BODY_SIZE 1048576

class Request
{
public:

	Request(void);
	Request(const Request& other);
	Request& operator=(const Request& other);
	~Request();

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
	void				setIsValid(const bool &value);
	void				setHeadDone(void);
	void				setMaxBodySize(std::size_t a_maxBody);

private:

	void	getRequestLine(std::string& line);
	void	createKeyValuePair(std::string line);

	bool								m_isValid;
	bool								m_headComplete;
	std::size_t							m_maxBodySize;
	std::string							m_head;
	std::string							m_body;
	std::map <std::string, std::string>	m_requestHeader;
};

#endif // !REQUEST_HPP

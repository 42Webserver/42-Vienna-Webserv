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

#include "Debug.hpp"

#define MAX_HEAD_SIZE 8192
#define MAX_BODY_SIZE 1073741824
//offset for "\\r\\n"
#define RN_CHAR_OFFSET 2
#define BASE_HEX 16

class Request
{
private:

	int									m_isValid;
	bool								m_headComplete;
	bool								m_bodyComplete;
	std::size_t							m_chunkSize;
	std::size_t							m_maxBodySize;
	std::string							m_head;
	std::string							m_body;
	std::string							m_buffer;
	std::map <std::string, std::string>	m_requestHeader;

	void	getRequestLine(std::string& line);
	void	createKeyValuePair(std::string line);

public:

	Request(void);
	Request(const Request& other);
	Request& operator=(const Request& other);
	~Request();

	const std::string&	getValue(const std::string& a_key);
	void				setUri(std::string a_uri);
	bool				getValue(const std::string& a_key, std::string& a_returnValue) const;
	void				setValue(const std::string& a_key, const std::string& a_val);
	std::string			getRequestHost() const;
	int					getIsValid(void) const;
	size_t				getContentLength() const;
	void				addHead(const std::string& a_head);
	bool				headComplete(void);
	void				addBody(const std::string& a_body);
	void 				reciveChunked(const std::string & a_body);
	bool				bodyComplete(void);
	bool				isReady(void);
	const std::string&	getHead();
	const std::string&	getBody();
	void				initMap();
	void				setHeadDone(void);
	void				setBodyDone(void);
	void				setMaxBodySize(std::size_t a_maxBody);
};

#endif // !REQUEST_HPP

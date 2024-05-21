#ifndef REQUEST_H
# define REQUEST_H

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>


class Request
{
	public:

		Request(void);
		Request(const Request& other);
		Request& operator=(const Request& other);
		~Request();
		Request(std::string& head, std::string& body, int fd);
		void    getRequestLine(std::string& line);
		void    createKeyValuePair(std::string & line);
		void    initMap(std::string head);
		int		getClientSocket() const;

	private:
		int									m_clientSocket;
		std::map <std::string, std::string>	m_requestHeader;

};

#endif // !REQUEST_H
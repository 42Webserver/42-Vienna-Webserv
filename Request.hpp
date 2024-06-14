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
		Request& operator=(const Request& other);
		~Request();
		Request(std::string& head, std::string& body, int fd);

		void    getRequestLine(std::string& line);
		void    createKeyValuePair(std::string line);
		void    initMap(std::string head);
		int		getClientSocket() const;
		const 	std::string& getValue(const std::string& a_key);
		bool	getValue(const std::string& a_key, std::string& a_returnValue) const;
		void	setValue(const std::string& a_key, const std::string& a_val);

		std::string getRequestHost() const;
		bool		getIsValid(void) const;

	private:
		bool								m_isValid;
		int									m_clientSocket;
		std::map <std::string, std::string>	m_requestHeader;

};

#endif // !REQUEST_HPP

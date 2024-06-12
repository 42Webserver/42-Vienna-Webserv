#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <map>
#include <vector>
#include "Server.hpp"
#include <poll.h>
#include "PollContainer.hpp"
#include <fstream>
#include "Data.hpp"
#include "ConfigParser.hpp"

class Webserver
{
private:

	std::vector<struct subserver>	m_subservers;
	std::vector<Server>				m_servers;
	PollContainer					m_polls;

public:

	Webserver(void);
	Webserver(std::vector<struct subserver> subservers);
	Webserver(const Webserver& other);
	Webserver& operator=(const Webserver& other);
	~Webserver();

	int	pollServers(void);
	int	pollClients(void);

	void	replaceWhitespaces(std::string& str);
	void	separateSpecialChars(std::string& str);
	void    removeComments(std::string& str);
	void	processLine(std::string& line, std::vector<std::string>& tokens);
	void	readConfigFile(const std::string& file);
	void	checkSyntax(std::vector<std::string>& tokens);
	void	removeHttpScope(std::vector<std::string>& tokens);
	void	sortConfigVector(std::vector<std::string>& tokens);

	void	checkValue(const std::string& key, std::vector<std::string>& value);
	void	checkValueAutoindex(std::string& value);
	void	checkValueRoot(std::string& value);
	void	checkValueClientMaxBodySize(std::string& value);
	void	checkValueReturn(std::vector<std::string>& value);
	void	checkValueAllowedMethods(std::vector<std::string>& value);



	int	run();
};


#endif // !WEBSERVER_HPP

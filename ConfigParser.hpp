#ifndef ConfigParser_HPP
# define ConfigParser_HPP

#include <map>
#include <vector>
#include <poll.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <cstdlib>
#include "Data.hpp"

class ConfigParser
{
private:

	static std::vector<struct subserver>	m_subservers;

	ConfigParser(void);
	ConfigParser(const ConfigParser& other);
	ConfigParser& operator=(const ConfigParser& other);
	~ConfigParser();

	static void	replaceWhitespaces(std::string& str);
	static void	separateSpecialChars(std::string& str);
	static void	removeComments(std::string& str);
	static void	processLine(std::string& line, std::vector<std::string>& tokens);
	static void	readConfigFile(const std::string& file);
	static void	checkSyntax(std::vector<std::string>& tokens);
	static void	removeHttpScope(std::vector<std::string>& tokens);
	static void	sortConfigVector(std::vector<std::string>& tokens);

	static void	checkValue(const std::string& key, std::vector<std::string>& value);
	static void	checkValueAutoindex(std::string& value);
	static void	checkValueClientMaxBodySize(std::string& value);
	static void	checkValueListen(std::vector<std::string>& value);
	static void	checkValueRoot(std::string& value);
	static void	checkValueReturn(std::vector<std::string>& value);
	static void	checkValueAllowedMethods(std::vector<std::string>& value);
	static void	checkErrorPage(std::vector<std::string>& value);
	static unsigned long ipToL(std::string ip);

	static void	setupErrorPages(struct subserver& subserver);

	static void	safeData(std::vector<std::string> tokens);
	static void	initSubserver(struct subserver &subserver);
	static void	initLocation(std::map<std::string, std::vector<std::string> > &location);
	static void	updateLocation(std::map<std::string, std::vector<std::string> > &location, struct subserver newSubserver);
	static void	printData(std::vector <struct subserver> data);

	static bool	getLocation(struct subserver &newSubserver, std::vector<std::string> &tokens, size_t &i);
	static bool	endOfServerScope(std::string &str, size_t &countScopes);
	static void	addValue(const std::vector<std::string> &tokens, struct subserver &newSubserver, size_t &i);

public:
	static std::vector<struct subserver>	parseConfig(std::string& configname);
};


#endif // !ConfigParser_HPP

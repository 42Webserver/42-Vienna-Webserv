#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

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
	static void checkValueUpload(std::string& value);
	static void	checkValueReturn(std::vector<std::string>& value);
	static void	checkValueAllowedMethods(std::vector<std::string>& value);
	static void	checkErrorPage(std::vector<std::string>& value);
	static void checkValueExtension(std::vector<std::string>& value);
	static void checkValueScriptPath(std::vector<std::string>& value);
	static void	setupErrorPages(struct subserver& subserver);
	static void checkDuplicateLocations(struct subserver& subserver);
	static void	safeData(std::vector<std::string> tokens);
	static void	compareCgiKey(const struct subserver& a_subserver);
	static void	initSubserver(struct subserver &subserver);
	static void	initLocation(t_config &location);
	static void	updateLocation(t_config &location, struct subserver newSubserver);
	static void	addValue(const std::vector<std::string> &tokens, struct subserver &newSubserver, size_t &i);
	static void allowAllMethods(std::vector<std::string> &value);
	static void basicRequirement(const t_config &serverConfig);
	static bool	getLocation(struct subserver &newSubserver, std::vector<std::string> &tokens, size_t &i);
	static bool	endOfServerScope(std::string &str, size_t &countScopes);

public:

	static std::vector<struct subserver>	parseConfig(std::string& configname);
};


#endif // !CONFIGPARSER_HPP

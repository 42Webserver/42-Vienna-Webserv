#include "ConfigParser.hpp"

std::vector<struct subserver> ConfigParser::m_subservers;

ConfigParser::ConfigParser(void) {}

ConfigParser::ConfigParser(const ConfigParser& a_other)
{
	(void)a_other;
}

ConfigParser&	ConfigParser::operator=(const ConfigParser& a_other)
{
	(void)a_other;
	return (*this);
}

ConfigParser::~ConfigParser() {}

void ConfigParser::replaceWhitespaces(std::string& str)
{
    std::string whitespaces = "\t\r\n\f\v ";

    for (std::string::iterator  it = str.begin(); it != str.end(); ++it)
    {
        if (whitespaces.find(*it) == std::string::npos)
            continue;
        else
        {
            std::string::iterator tmp = it;
            for (; tmp != str.end(); ++tmp)
            {
                if (whitespaces.find(*tmp) == std::string::npos)
                    break;
            }
            str.erase(it + 1, tmp);
            if (*it != ' ')
                *it = ' ';
        }
    }
}

void	ConfigParser::separateSpecialChars(std::string& str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (i != str.length() - 1 && str.at(i) == ';' && (str.at(i + 1) != ' '))
			str.insert(str.begin() + i + 1, ' ');
		if (str.at(i) == ';' && (i != 0 && str.at(i - 1) != ' '))
			str.insert(str.begin() + i, ' ');
		else if (str.at(i) == '{' && (i != 0 && str.at(i - 1) != ' '))
			str.insert(str.begin() + i, ' ');
		else if (str.at(i) == '}' && (i != 0 && str.at(i - 1) != ' '))
			str.insert(str.begin() + i, ' ');
	}
}

void    ConfigParser::removeComments(std::string& str)
{
    size_t hashtag;
    if ((hashtag = str.find('#')) != std::string::npos)
        str.erase(hashtag, str.length());
}

void	ConfigParser::processLine(std::string& line, std::vector<std::string>& tokens)
{
	if (line.empty())
		return;

	removeComments(line);
	replaceWhitespaces(line);
	separateSpecialChars(line);

	if (line.length() == 1 && line.at(0) == ' ')
		return;

	std::stringstream	stream(line);

	std::string current;

	int checkBegin = 0;
	while (std::getline(stream, current, ' '))
	{
		if (current == ";" && checkBegin == 0)
			throw(std::runtime_error("Error: config-file: line begins with semicolon''."));
		if (!current.empty())
		{
			tokens.push_back(current);
			checkBegin++;
		}
	}

	if (checkBegin > 1 && line.find_first_of("{}") != std::string::npos)
		throw(std::runtime_error("Error: config-file: invalid characters next to braces."));


	if (checkBegin >= 2 && tokens.at(tokens.size() - checkBegin) == "location")
	{
		if (checkBegin != 2 || tokens.back().at(0) != '/')
			throw(std::runtime_error("Error: config-file: invalid location."));
		return;
	}

	if (((tokens.back() != ";") && (tokens.back() != "http" && tokens.back() != "Server" && tokens.back() != "{" && tokens.back() != "}")))
		throw(std::runtime_error("Error: config-file: invalid line without semicolon."));

	else if (tokens.back() == ";")
	{
		std::string secondLast;

		if (tokens.size() >= 2)
			secondLast = tokens.at(tokens.size() - 2);

		if (secondLast.empty())
			throw(std::runtime_error("Error: config-file: invalid line with semicolon."));

		if (secondLast == "http" || secondLast == "Server" || secondLast == "{" || secondLast == "}")
			throw(std::runtime_error("Error: config-file: semicolon after special token (http / Server / { / })."));
	}
}

void	ConfigParser::checkSyntax(std::vector<std::string>& tokens)
{
	std::vector<std::string>::iterator	it;
	int									braceCount = 0;
	int 								maxBraceCount = 0;
	for (it = tokens.begin(); it != tokens.end(); ++it)
	{
		if (it != tokens.begin() && *it == "http")
			throw(std::runtime_error("Error: config-file: invalid http format."));

		if (braceCount > maxBraceCount)
			maxBraceCount = braceCount;

		if (braceCount < 0)
			throw(std::runtime_error("Error: config-file: invalid braces."));

		if (*it == "http" && braceCount != 0)
			throw(std::runtime_error("Error: config-file: http in wrong scope."));

		if (*it == "Server" && braceCount != 1)
			throw(std::runtime_error("Error: config-file: Server in wrong scope."));

		if (braceCount == 1 && *(it - 1) == "}" && *it == "{")
			throw(std::runtime_error("Error: config-file: missing Server."));

		if (*it == "location" && braceCount != 2)
			throw(std::runtime_error("Error: config-file: location in wrong scope."));

		if (*it == "{" || *it == "}")
			*it == "{" ? braceCount++ : braceCount--;

		if (*it == "{" && (it + 1 == tokens.end() || *(it + 1) == "}"))
			throw(std::runtime_error("Error: config-file: empty scope."));

		if (it != tokens.begin() && *it == ";" && *(it - 1) == ";")
			throw(std::runtime_error("Error: config-file: consecutive semicolons."));

		if (*it == "location" && ((it + 2) == tokens.end() || *(it + 2) != "{"))
			throw(std::runtime_error("Error: config-file: invalid location scope."));

		if (*it == "Server" && ((it + 1) == tokens.end() || *(it + 1) != "{"))
			throw(std::runtime_error("Error: config-file: invalid Server scope."));
	}
	if (maxBraceCount > 3 || maxBraceCount < 2)
		throw(std::runtime_error("Error: config-file: invalid format."));
	if (braceCount != 0)
		throw(std::runtime_error("Error: config-file: invalid braces."));
}

void	ConfigParser::sortConfigVector(std::vector<std::string>& tokens)
{
	std::vector<std::string>						sortedTokens;
	std::vector<std::vector<std::string>::iterator>	locations;

	for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		if (*it == "}")
		{
			while (locations.size() > 0)
			{
				std::vector<std::string>::iterator i = locations.front();
				while (*i != "}")
					sortedTokens.push_back(*(i++));
				sortedTokens.push_back(*(i++));
				locations.erase(locations.begin());
			}
			sortedTokens.push_back(*it);
			continue;
		}
		if (*it != "location")
			sortedTokens.push_back(*it);
		else
		{
			locations.push_back(it);
			while (*it != "}")
				it++;
		}
	}
	tokens = sortedTokens;
}

void	ConfigParser::removeHttpScope(std::vector<std::string>& tokens)
{
	if (tokens.size() < 3)
		throw(std::runtime_error("Error: config-file: too few arguments."));
	if (tokens.at(0) != "http" || tokens.at(1) != "{" || tokens.back() != "}")
		throw(std::runtime_error("Error: config-file: invalid http format."));

	tokens.pop_back();
	tokens.erase(tokens.begin(), tokens.begin() + 2);
}

void	ConfigParser::readConfigFile(const std::string& file)
{
	std::ifstream inFile(file.c_str());

	if (!inFile.is_open())
		throw(std::runtime_error("Error: config-file: could not open."));

	std::stringstream buffer;
	buffer << inFile.rdbuf();

	if (buffer.str().empty())
		throw(std::runtime_error("Error: config-file: file is empty."));

	std::vector<std::string>	tokens;
	std::string 				line;

	while (std::getline(buffer, line, '\n'))
		processLine(line, tokens);

	checkSyntax(tokens);
	removeHttpScope(tokens);
	sortConfigVector(tokens);

	safeData(tokens);
	printData(m_subservers);

	inFile.close();
}

void	ConfigParser::checkValueAutoindex(std::string& value)
{
	if (value != "on" && value != "off")
		throw(std::runtime_error("Error: config-file: invalid value at key 'autoindex'."));
}

void	ConfigParser::checkValueClientMaxBodySize(std::string& value)
{
	if (value.find_first_not_of("0123456789") != std::string::npos)
		throw(std::runtime_error("Error: config-file: invalid value at key 'client_max_body_size'."));
}

unsigned long ConfigParser::ipToL(std::string ip)
{
	std::string octet;
	std::istringstream input(ip);
	unsigned long result = 0;
	unsigned long value;
	int shiftSize = 24;
	if (ip.find_first_not_of("0123456789.") != std::string::npos)
		throw std::runtime_error("Error: config-file: invalid ip at key 'listen'.");
	while (std::getline(input, octet, '.') && shiftSize >= 0)
	{
    	if (octet.length() <= 0 || octet.length() > 3 || strtol(octet.c_str(), NULL, 10) > 255)
			throw std::runtime_error("Error: config-file: invalid ip at key 'listen'.");
		value = strtol(octet.c_str(), NULL, 10);
		result +=	value << shiftSize;
		shiftSize -= 8;
	}
    if (shiftSize != -8)
		throw std::runtime_error("Error: config-file: invalid ip at key 'listen'.");
	return (result);
}

void	ConfigParser::checkValueListen(std::vector<std::string>& value)
{
	if (value.at(0).find_first_not_of("0123456789") == std::string::npos)
	{
		long	nb = std::atol(value.at(0).c_str());

		if (nb > std::numeric_limits<unsigned short>::max())
			throw(std::runtime_error("Error: config-file: invalid port at key 'listen'."));
		value.push_back(value.at(0));
		value.at(0) = "0";
		return;
	}

	size_t	last = value.at(0).find_last_of(":");

	if (last == std::string::npos)
		throw(std::runtime_error("Error: config-file: invalid port at key 'listen'."));

	std::string	ip = value.at(0).substr(0, last);
	std::string	port = value.at(0).substr(last + 1, value.at(0).length() - last);

	if (ip != "localhost" && ip != "[::]")
	{
		std::stringstream ipStr;
		ipStr << ipToL(ip);
		value.at(0) = ipStr.str();
	}

	if (port.find_first_not_of("0123456789") == std::string::npos)
	{
		long	nb = std::atol(port.c_str());

		if (nb > std::numeric_limits<unsigned short>::max())
			throw(std::runtime_error("Error: config-file: invalid port at key 'listen'."));

		if (ip == "localhost" || ip == "[::]")
			value.at(0) = "0";

		value.push_back(port);
		return;
	}
}

void	ConfigParser::checkValueRoot(std::string& value)
{
	if (value.at(0) != '/')
		throw(std::runtime_error("Error: config-file: invalid value at key 'root'."));
}

void	ConfigParser::checkValueAllowedMethods(std::vector<std::string>& value)
{
	if (value.size() > 3)
		throw(std::runtime_error("Error: config-file: too many values at key 'allowed_methods'."));

	for (size_t i = 0; i < value.size(); ++i)
	{
		if (value.at(i) != "GET" && value.at(i) != "POST" && value.at(i) != "DELETE")
			throw(std::runtime_error("Error: config-file: invalid value at key 'allowed_methods'."));
		if (i == 1 && value.at(1) == value.at(0))
			throw(std::runtime_error("Error: config-file: double value at key 'allowed_methods'."));
		if (i == 2 && (value.at(2) == value.at(0) || value.at(2) == value.at(1)))
			throw(std::runtime_error("Error: config-file: double value at key 'allowed_methods'."));
	}
}

void	ConfigParser::checkValueReturn(std::vector<std::string>& value)
{
	if (value.size() > 2)
		throw(std::runtime_error("Error: config-file: too many values at key 'return'."));

	if (value.at(0).length() > 3 || value.at(0).find_first_not_of("0123456789") != std::string::npos)
		throw(std::runtime_error("Error: config-file: invalid statuscode at key 'return'."));

	if (value.size() == 2 && value.at(1).at(0) != '/')
		throw(std::runtime_error("Error: config-file: invalid location at key 'return'."));
}

void	ConfigParser::checkValue(const std::string& key, std::vector<std::string>& value)
{
	if (key == "autoindex")
		checkValueAutoindex(value.at(0));
	if (key == "client_max_body_size")
		checkValueClientMaxBodySize(value.at(0));
	if (key == "root")
		checkValueRoot(value.at(0));
	if (key == "allowed_methods")
		checkValueAllowedMethods(value);
	if (key == "return")
		checkValueReturn(value);
	if (key == "listen")
		checkValueListen(value);
}

// shit incoming

void	ConfigParser::printData(std::vector <struct subserver> data)
{
	for (size_t i = 0; i < data.size(); i++)
	{
		std::cout << "Server number " << i << std::endl;
		for (std::map<std::string, std::vector<std::string> > ::iterator it = data.at(i).server.begin(); it != data.at(i).server.end(); ++it)
		{
			std::cout << "	Key: " << it->first << " | value: ";
			for (size_t i = 0; i < it->second.size(); i++)
			{
				std::cout << it->second.at(i) << ", ";
			}
			std::cout<< std::endl;
		}
		std::cout << "Location: \n";

		for (size_t j = 0; j < data.at(i).locations.size(); j++)
		{
			std::cout << "Location number: " << j << std::endl;
			for (std::map<std::string, std::vector<std::string> > ::iterator it = data.at(i).locations.at(j).begin(); it != data.at(i).locations.at(j).end(); ++it)
			{
				std::cout << "		Key: " << it->first << " | value: ";
				for (size_t j = 0; j < it->second.size(); j++)
				{
					std::cout << it->second.at(j) << ", ";
				}
				std::cout<< std::endl;
			}
		}			struct subserver newSubserver;

	}
}

bool	ConfigParser::getLocation(struct subserver &newSubserver, std::vector<std::string> &tokens, size_t &i)
{
	std::map< std::string, std::vector<std::string> > location;
	initLocation(location);
	std::vector<std::string> value;
	value.push_back(tokens.at(i + 1));
	location["name"] = value;
	i += 3;
	while(tokens.at(i) != "}")
	{
		value.clear();
		if (tokens.at(i) == "listen" || tokens.at(i) == "root" || tokens.at(i) == "index" \
			|| tokens.at(i) == "client_max_body_size" || tokens.at(i) == "autoindex")
		{
			value.push_back(tokens.at(++i));
			checkValue(tokens.at(i - 1), value);
			if (location[tokens.at(i - 1)].empty())
				location[tokens.at(i - 1)] = value;
			if (tokens.at(++i) != ";")
				throw std::runtime_error("Error: config-file: Two many arguments for key [locationscope]");
		}
		else if (tokens.at(i) == "server_name" || tokens.at(i) == "error_page" || \
			tokens.at(i) == "allowed_methods" || tokens.at(i) == "return")
		{
			i++;
			while (tokens.at(i) != ";")
				value.push_back(tokens.at(i++));
			checkValue(tokens.at(i - value.size() - 1), value);
			if (location[tokens.at(i - value.size() - 1)].empty())
				location[tokens.at(i - value.size() - 1)] = value;
		}
		else
			throw std::runtime_error("Error: config-file: Trash not allowed [locationscope]");
		i++;
	}
	updateLocation(location, newSubserver);
	newSubserver.locations.push_back(location);
	return (true);
}

bool	ConfigParser::endOfServerScope(std::string &str, size_t &countScopes)
{
		str == "{" ? countScopes++ : countScopes--;
		if (countScopes == 0)
			return (true);
		return false;
}

void	ConfigParser::addValue(const std::vector<std::string> &tokens, struct subserver &newSubserver, size_t &i)
{
	std::vector<std::string> value;

	if (tokens.at(i + 1) == ";")
		throw std::runtime_error("Error: config-file: Missing argument at key [serverscope]");
	//ADD just one arg!
	if (tokens.at(i) == "listen" || tokens.at(i) == "root" || tokens.at(i) == "index" \
		|| tokens.at(i) == "client_max_body_size" || tokens.at(i) == "autoindex")
	{
		i++;
		value.push_back(tokens.at(i));
		checkValue(tokens.at(i - 1), value);
		if (newSubserver.server[tokens.at(i - 1)].empty())
			newSubserver.server[tokens.at(i - 1)] = value;
		if (tokens.at(++i) != ";")
			throw std::runtime_error("Error: config-file: Two many arguments for key [serverscope]");
	}
	//ADD optional multiple arguments!
	else if (tokens.at(i) == "server_name" || tokens.at(i) == "error_page" || \
		tokens.at(i) == "allowed_methods" || tokens.at(i) == "return")
	{
		i++;
		while (tokens.at(i) != ";")
			value.push_back(tokens.at(i++));
		checkValue(tokens.at(i - value.size() - 1), value);
		if (newSubserver.server[tokens.at(i - value.size() - 1)].empty())
			newSubserver.server[tokens.at(i - value.size() - 1)] = value;
	}
}

void ConfigParser::safeData(std::vector<std::string> tokens)
{
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens.at(i) == "Server")
		{
			i += 2;
			struct subserver newSubserver;
			initSubserver(newSubserver);
			size_t countScopes = 1;
			for (; i < tokens.size(); i++)
			{
				if (tokens.at(i) == "{" || tokens.at(i) == "}")
				{
					if (endOfServerScope(tokens.at(i), countScopes))
						break;
					else
						continue;
				}
				if (newSubserver.server.find(tokens.at(i)) != newSubserver.server.end()) //add value
					addValue(tokens, newSubserver, i);
				else if (tokens.at(i) == "location") 		//Add location
					getLocation(newSubserver, tokens, i);
				else
					throw std::runtime_error("Error: config-file: Trash not allowed [serverscope]");
			}
			m_subservers.push_back(newSubserver);
		}
		else
			throw std::runtime_error("Error: config-file: Trash between scopes!");
	}
}

void ConfigParser::initSubserver(struct subserver &subserver)
{
	subserver.server["listen"];
	subserver.server["root"];
	subserver.server["index"];
	subserver.server["server_name"];
	subserver.server["client_max_body_size"];
	subserver.server["error_page"];
	subserver.server["allowed_methods"];
	subserver.server["autoindex"];
	subserver.server["return"];
	subserver.server["locations"];
}

void ConfigParser::updateLocation(std::map<std::string, std::vector<std::string> > &location, struct subserver newSubserver)
{
	std::map<std::string, std::vector<std::string> >::iterator	it = location.begin();

	while (it != location.end())
	{
		if ((it)->second.empty())
			it->second = newSubserver.server[it->first];
		it++;
	}
}

void ConfigParser::initLocation(std::map<std::string, std::vector<std::string> > &location)
{
	location["index"];
	location["client_max_body_size"];
	location["error_page"];
	location["allowed_methods"];
	location["autoindex"] ;
	location["return"];
	location["root"];
	location["name"];
}

std::vector<struct subserver>	ConfigParser::parseConfig(std::string& configname)
{
	readConfigFile(configname);
	return (m_subservers);
}

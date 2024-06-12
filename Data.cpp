#include "Data.hpp"

void printData(std::vector <struct subserver> data)
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
		}
	}
}

bool getLocation(struct subserver &newSubserver, std::vector<std::string> &tokens, size_t &i)
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

std::vector<struct subserver> safeData(std::vector<std::string> tokens)
{
	std::vector<struct subserver> data;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens.at(i) == "Server")
		{
			i += 2;
			struct subserver newSubserver;
			initSubserver(newSubserver);
			//Already skip the opening brackets of serverscope
			int countScopes = 1;
			for (; i < tokens.size(); i++)
			{
				if (tokens.at(i) == "{" || tokens.at(i) == "}")
				{
					tokens.at(i) == "{" ? countScopes++ : countScopes--;
					if (countScopes == 0)
						break;
					continue;
				}
				//key = string
				if (newSubserver.server.find(tokens.at(i)) != newSubserver.server.end())
				{
					//Add exactly one
					std::vector<std::string> value;
					if (tokens.at(i + 1) == ";")
						throw std::runtime_error("Error: config-file: Missing argument at key [serverscope]");
					//ADD just one arg!
					if (tokens.at(i) == "listen" || tokens.at(i) == "root" || tokens.at(i) == "index" \
						|| tokens.at(i) == "client_max_body_size" || tokens.at(i) == "autoindex")
					{
						i++;
						value.push_back(tokens.at(i));
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
						if (newSubserver.server[tokens.at(i - value.size() - 1)].empty())
							newSubserver.server[tokens.at(i - value.size() - 1)] = value;
					}
					//Add location
				}
				else if (tokens.at(i) == "location")
					getLocation(newSubserver, tokens, i);
				else
					throw std::runtime_error("Error: config-file: Trash not allowed [serverscope]");
			}
			data.push_back(newSubserver);
		}
		else
			throw std::runtime_error("Error: config-file: Trash between scopes!");
	}
	printData(data);
	std::cout << "Number of subservers: " << data.size() << std::endl;
	return (data);
}

void initSubserver(struct subserver &subserver)
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

void updateLocation(std::map<std::string, std::vector<std::string> > &location, struct subserver newSubserver)
{
	std::map<std::string, std::vector<std::string> >::iterator	it = location.begin();

	while (it != location.end())
	{
		if ((it)->second.empty())
			it->second = newSubserver.server[it->first];
		it++;
	}
}

void initLocation(std::map<std::string, std::vector<std::string> > &location)
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

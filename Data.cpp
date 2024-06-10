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
		
	for (size_t j = 0; j < data.at(i).location.size(); j++)
	{
		std::cout << "Location number: " << j << std::endl;
		for (std::map<std::string, std::vector<std::string> > ::iterator it = data.at(i).location.at(j).begin(); it != data.at(i).location.at(j).end(); ++it)
		{
			std::cout << "	Key: " << it->first << " | value: ";
			for (size_t j = 0; j < it->second.size(); j++)
			{
				std::cout << it->second.at(j) << ", ";
			}
			std::cout<< std::endl;
		}
		std::cout << "Location: \n";
		
		
	}
		
	}
	
}

bool safeData(std::vector<std::string> tokens)
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
					{
						std::cout << "Finished server scope! | i = "<< i <<  std::endl;
						break;
					}
					continue;
				}
				//key = string
				if (newSubserver.server.find(tokens.at(i)) != newSubserver.server.end())
				{
					//Add exactly one
					if (tokens.at(i + 1) == ";")
					{
						std::cout << "Missing argument at key = " << tokens.at(i) << std::endl;
						return (false);
					}

					//ADD just one arg!
					if (tokens.at(i) == "listen" || tokens.at(i) == "root" || tokens.at(i) == "index" \
						|| tokens.at(i) == "client_max_body_size" || tokens.at(i) == "autoindex")
					{
						i++;
						std::vector <std::string> value;
						value.push_back(tokens.at(i));
						newSubserver.server[tokens.at(i - 1)] = value;
						i++;
						if (tokens.at(i) != ";")
						{
							std::cerr << "Two many arguments for key: " << tokens.at(i - 2);
							return (false);
						}
					}
					//ADD optional multiple arguments!
					else if (tokens.at(i) == "server_name" || tokens.at(i) == "error_page" || \
						tokens.at(i) == "allowed_methods" || tokens.at(i) == "return")
					{
						i++;
						std::vector<std::string> value;
						while (tokens.at(i) != ";")
							value.push_back(tokens.at(i++));
						newSubserver.server[tokens.at(i - value.size() - 1)] = value;
					}
					//Add location
					}
					else if (tokens.at(i) == "location")
					{
						std::map< std::string, std::vector<std::string> > location;
						initLocation(location, newSubserver);
						std::cout << "Found a location! and skip it now!" << std::endl;
						i += 3;




						while(tokens.at(i) != "}")
						{
							if (tokens.at(i) == "listen" || tokens.at(i) == "root" || tokens.at(i) == "index" \
								|| tokens.at(i) == "client_max_body_size" || tokens.at(i) == "autoindex")
							{
								i++;
								std::vector <std::string> value;
								value.push_back(tokens.at(i));
								location[tokens.at(i - 1)] = value;
								i++;
								if (tokens.at(i) != ";")
								{
									std::cerr << "Two many arguments in location for key: " << tokens.at(i - 2);
									return (false);
								}
							}
							else if (tokens.at(i) == "server_name" || tokens.at(i) == "error_page" || \
								tokens.at(i) == "allowed_methods" || tokens.at(i) == "return")
							{
								i++;
								std::vector<std::string> value;
								while (tokens.at(i) != ";")
									value.push_back(tokens.at(i++));
								location[tokens.at(i - value.size() - 1)] = value;
							}
							else 
							{
								std::cout << "Trash not allowed in location: " << tokens.at(i) <<  std::endl;
								std::cout << "Befor trash" << tokens.at(i - 1) << std::endl;
								return (false);
							}
							i++;
						}



						newSubserver.location.push_back(location);
						// newSubserver[""]
					}
					else 
					{
						std::cout << "Trash not allowed: " << tokens.at(i) <<  std::endl;
						std::cout << "Befor trash" << tokens.at(i - 1) << std::endl;
						return (false);
					}
			}
			data.push_back(newSubserver);
		}
		else 
		{
			std::cout << "Trash between scopes!: " << tokens.at(i) << "pos: " << i << std::endl;
			return (false);
		}

	}

	printData(data);
	std::cout << "Number of subservers: " << data.size() << std::endl;
	return false;
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

void initLocation(std::map<std::string, std::vector<std::string> > &location, struct subserver newSubserver)
{
	location["index"] = newSubserver.server["index"];
	location["client_max_body_size"] = newSubserver.server["client_max_body_size"];
	location["error_page"] = newSubserver.server["error_page"];
	location["allowed_methods"] = newSubserver.server["allowed_methods"];
	location["autoindex"] = newSubserver.server["autoindex"];
	location["return"] = newSubserver.server["return"];
	location["root"] = newSubserver.server["root"];

}

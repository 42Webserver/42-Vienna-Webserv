#include "Data.hpp"

void printData(std::vector <std::map< std::string, std::vector<std::string> > > data)
{
	for (size_t i = 0; i < data.size(); i++)
	{
		std::cout << "Server number " << i << std::endl;
		for (std::map<std::string, std::vector<std::string> > ::iterator it = data.at(i).begin(); it != data.at(i).end(); ++it)
		{
			std::cout << "	Key: " << it->first << " | value: ";
			for (size_t i = 0; i < it->second.size(); i++)
			{
				std::cout << it->second.at(i) << ", ";
			}
			std::cout<< std::endl;
		}
		
	}
	
}

bool safeData(std::vector<std::string> tokens)
{
	std::vector<std::map<std::string, std::vector<std::string> > > data;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens.at(i) == "Server")
		{
			i += 2;
			std::map<std::string, std::vector<std::string> > newSubserver;
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
				if (newSubserver.find(tokens.at(i)) != newSubserver.end())
				{
					//Add exactly one
					if (tokens.at(i + 1) == ";")
					{
						std::cout << "Missing argument at key = " << tokens.at(i) << std::endl;
						return (false);
					}

					//ADD just one arg!
					if (tokens.at(i) == "listen" || tokens.at(i) == "root" || tokens.at(i) == "index" \
						|| tokens.at(i) == "client_max_body_size")
					{
						i++;
						std::vector <std::string> value;
						value.push_back(tokens.at(i));
						newSubserver[tokens.at(i - 1)] = value;
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
						newSubserver[tokens.at(i - value.size() - 1)] = value;
					}
					//Add location
					}
					else if (tokens.at(i) == "location")
					{
						std::cout << "Found a location! and skip it now!" << std::endl;
						while(tokens.at(i) != "}")
						i++;
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

void initSubserver(std::map <std::string, std::vector<std::string> > &subserver)
{
	subserver["listen"];
	subserver["root"];
	subserver["index"];
	subserver["server_name"];
	subserver["client_max_body_size"];
	subserver["error_page"];
	subserver["allowed_methods"];
	subserver["autoindex"];
	subserver["return"];
	subserver["locations"];

}


#include "Data.hpp"
#include <iostream>

u_int16_t	subserver::getPort(void) const 
{
	return (atoi(serverConfig.at("listen").at(1).c_str()));
}

u_int64_t subserver::getIp(void) const
{
	return (std::strtol(serverConfig.at("listen").at(0).c_str(), NULL, 10));
}

std::string subserver::getHost(void) const
{
    return serverConfig.at("listen").at(0);
}

serveradress subserver::getAdress(void) const
{
	return (serveradress(getIp(), getPort()));
}

const t_config &subserver::getValidConfig(std::string a_uri)
{
	size_t pos;

	while ((pos = a_uri.find_last_of("/")) != std::string::npos)
	{
		for (size_t i = 0; i < locationConfigs.size(); i++)
		{
			if (locationConfigs.at(i).at("name").at(0) == a_uri)
				return locationConfigs.at(i);
		}
		if (pos + 1 == a_uri.length())
			a_uri = a_uri.substr(0, pos);
		else 
			a_uri = a_uri.substr(0, pos + 1);
	}
	return (serverConfig);
}

u_int64_t serveradress::getIp(void) const
{
	return (ip);
}

u_int16_t serveradress::getPort(void) const
{
	return (port);
}

serveradress::serveradress(u_int64_t a_ip, u_int16_t a_port) : ip(a_ip), port(a_port) {}

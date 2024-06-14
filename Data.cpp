#include "Data.hpp"

u_int16_t	subserver::getPort(void) const 
{
	return (atoi(serverConfig.at("listen").at(1).c_str()));
}

u_int64_t subserver::getIp(void) const
{
	return (std::strtol(serverConfig.at("listen").at(0).c_str(), NULL, 10));
}

serveradress subserver::getAdress(void) const
{
	return (serveradress(getIp(), getPort()));
}

const t_config &subserver::getValidConfig(const std::string& a_uri) const
{
	for (size_t i = 0; i < locationConfigs.size(); i++)
	{
		if (locationConfigs.at(i).at("name").at(0) == a_uri)
			return (locationConfigs.at(i));
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

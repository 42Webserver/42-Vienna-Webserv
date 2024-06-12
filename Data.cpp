#include "Data.hpp"

int	subserver::getPort(void) const 
{
	return (atoi(serverConfig.at("listen").at(1).c_str()));
}

u_long subserver::getIp(void) const
{
	return (std::strtol(serverConfig.at("listen").at(0).c_str(), NULL, 10));
}

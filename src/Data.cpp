#include "Data.hpp"
#include <iostream>

u_int16_t	subserver::getPort(void) const 
{
	return (std::atoi(serverConfig.at("listen").at(1).c_str()));
}

u_int64_t subserver::getIp(void) const
{
	return (std::strtol(serverConfig.at("listen").at(0).c_str(), NULL, 10));
}

void	subserver::setIp(u_int32_t n_host)
{
	std::stringstream ss; 
	ss << ntohl(n_host);
	serverConfig.at("listen").at(0) = ss.str();
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


addrinfo* subserver::setServerAdress(void)
{
	int error_code;
	struct addrinfo input, *result; 
	
	memset(&input, 0, sizeof(input));
	input.ai_family = AF_INET; //IPV4;
	input.ai_socktype = SOCK_STREAM; //TCP connection; 
	if ((error_code = getaddrinfo(getHost().c_str(), "http", &input, &result)) != 0)
		throw (std::runtime_error("Error: server: Host not found"));
	
	setIp(((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr);
	
	return (result);
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

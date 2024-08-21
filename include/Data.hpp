#ifndef DATA_H
# define DATA_H

#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <ostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <sstream>

typedef std::map<std::string, std::vector<std::string> > t_config;

struct serveradress
{
private:

	u_int64_t	ip;
	u_int16_t	port;

public:
	
	serveradress(u_int64_t a_ip, u_int16_t a_port);
	u_int64_t getIp(void) const;
	u_int16_t getPort(void) const;
};

struct subserver
{
    t_config				serverConfig;
    std::vector <t_config> locationConfigs;

	u_int16_t		getPort(void) const;
	u_int64_t		getIp(void) const;
	void			setIp(u_int32_t n_host);
	std::string		getHost(void) const;
	serveradress	getAdress(void) const;
	const t_config&	getValidConfig(std::string a_uri);
	addrinfo*		setServerAdress(void);
};

#endif // !DATA_H

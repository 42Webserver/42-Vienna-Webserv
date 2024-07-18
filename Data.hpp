#ifndef DATA_H
# define DATA_H

#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <ostream>

typedef std::map<std::string, std::vector<std::string> > t_config;

struct serveradress
{
private:
	u_int64_t	ip;
	u_int16_t	port;
public:
	u_int64_t getIp(void) const;
	u_int16_t getPort(void) const;
	serveradress(u_int64_t a_ip, u_int16_t a_port);

};

struct subserver
{
    std::map< std::string, std::vector<std::string> > serverConfig;
    std::vector <std::map< std::string, std::vector<std::string> > > locationConfigs;

	u_int16_t		getPort(void) const;
	u_int64_t		getIp(void) const;
	std::string		getHost(void) const;
	serveradress	getAdress(void) const;
	const t_config&	getValidConfig(std::string a_uri);
};

#endif // !DATA_H

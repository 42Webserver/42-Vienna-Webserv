#ifndef DATA_H
# define DATA_H

#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <ostream>

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
	serveradress	getAdress(void) const;
};

#endif // !DATA_H

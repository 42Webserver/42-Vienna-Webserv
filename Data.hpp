#ifndef DATA_H
# define DATA_H

#include <string>
#include <vector>
#include <map>
#include <cstdlib>

struct subserver
{
    std::map< std::string, std::vector<std::string> > serverConfig;
    std::vector <std::map< std::string, std::vector<std::string> > > locationConfigs;

	int	getPort(void) const;
	u_long getIp(void) const;
};

#endif // !DATA_H

#ifndef DATA_H
# define DATA_H

#include <string>
#include <vector>
#include <map>

struct subserver
{
    std::map< std::string, std::vector<std::string> > server;
    std::vector <std::map< std::string, std::vector<std::string> > > locations;
};

#endif // !DATA_H

#ifndef DATA_H
# define DATA_H

#include <string>
#include <vector>
#include <iostream>
#include <map>

struct subserver
{
    std::map< std::string, std::vector<std::string> >server;
    std::vector <std::map< std::string, std::vector<std::string> > > location;
};
bool    safeData(std::vector<std::string> tokens);
void    initSubserver(struct subserver &subserver);
void    initLocation(std::map<std::string, std::vector<std::string> > &location, struct subserver newSubserver);

#endif // !DATA_H

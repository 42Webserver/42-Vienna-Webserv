#ifndef DATA_H
# define DATA_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <exception>

struct subserver
{
    std::map< std::string, std::vector<std::string> > server;
    std::vector <std::map< std::string, std::vector<std::string> > > locations;
};

std::vector<struct subserver>    safeData(std::vector<std::string> tokens);
void    initSubserver(struct subserver &subserver);
void    initLocation(std::map<std::string, std::vector<std::string> > &location);
void    updateLocation(std::map<std::string, std::vector<std::string> > &location, struct subserver newSubserver);
void    printData(std::vector <struct subserver> data);

#endif // !DATA_H

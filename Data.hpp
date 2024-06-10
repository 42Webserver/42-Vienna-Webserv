#ifndef DATA_H
# define DATA_H

#include <string>
#include <vector>
#include <iostream>
#include <map>

struct location
{
    std::string root;
    std::string index; 
    int client_max_body_size;
    std::vector<std::string> error_pages;
    std::vector<std::string> allowed_methods;
    bool autoindex;
    std::vector<std::string> redir; 
};

struct subserver
{
    std::map< std::string, std::vector<std::string> >server;
    std::vector <std::map< std::string, std::vector<std::string> > > location;
};
bool    safeData(std::vector<std::string> tokens);
void    initSubserver(struct subserver &subserver);
void    initLocation(std::map<std::string, std::vector<std::string> > &location, struct subserver newSubserver);

#endif // !DATA_H

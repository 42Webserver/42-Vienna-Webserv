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
    std::string listen; //1
    std::string root;   // 1
    std::string index; // 1
    std::vector<std::string> server_name; //min 1
    int client_max_body_size;   //1
    std::vector<std::string> error_page; //min 1
    std::vector<std::string> allowed_methods; //min 1
    bool autoindex; //on || off
    std::vector<std::string> redir; //min 1 
    std::vector<location> locations; //optional
};


bool    safeData(std::vector<std::string> tokens);
void    initSubserver(std::map <std::string, std::vector<std::string> > &subserver);

#endif // !DATA_H

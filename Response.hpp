#ifndef RESPONSE_H
# define RESPONSE_H

#include <string>
#include <iostream>
#include <map>
#include "Dummy_Request.hpp" 

class Response
{
private:

    std::string     m_responseMsg;
    Dummy_Request   m_request;

public:

    Response(void);
    Response(const Dummy_Request& a_request);
    Response(const Response& other);
    Response& operator=(const Response& other);
    ~Response();

    void createResponseMessage();
};

#endif // !RESPONSE_H

#ifndef DUMMY_REQUEST_H
# define DUMMY_REQUEST_H

#include <map>
#include <string>
//#include "Response.hpp"

class Dummy_Request
{
    public:

        Dummy_Request(std::map<std::string, std::string> a_request);
        Dummy_Request(const Dummy_Request& other);
        Dummy_Request& operator=(const Dummy_Request& other);
        ~Dummy_Request();
        std::string const & getValue(std::string const & key) const;
        Dummy_Request(void);

    private:

        std::map <std::string, std::string> m_request;

};

#endif // !DUMMY_REQUEST_H

#include "Dummy_Request.hpp"

Dummy_Request::Dummy_Request(void)
{
}

Dummy_Request::Dummy_Request(std::map<std::string, std::string> a_request)
{
    m_request = a_request;
}

Dummy_Request::Dummy_Request(const Dummy_Request &other)
{
    *this = other;
}

Dummy_Request &Dummy_Request::operator=(const Dummy_Request &other)
{
    if (this != &other)
    {
        m_request = other.m_request;
    }
    return (*this);
}

Dummy_Request::~Dummy_Request()
{
}

std::string const &Dummy_Request::getValue(std::string const &key) const
{
    return (m_request.find(key)->second);
}

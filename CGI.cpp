#include "CGI.hpp"

CGI::CGI() {}

CGI::CGI(const CGI &other)
{
	*this = other;
}

CGI &CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		*this = other;
	}
	return (*this);
}

CGI::~CGI() {}

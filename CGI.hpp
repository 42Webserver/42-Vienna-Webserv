#ifndef CGI_HPP
# define CGI_HPP

#include <string>
#include <iostream>

class CGI
{
private:

public:
	CGI(void);
	CGI(const CGI& other);
	CGI& operator=(const CGI& other);
	~CGI();
};

#endif // !CGI_HPP

#include "bitmask.hpp"

int main () 
{
	int eventFlags = 0;

	if (eventFlags & AUTOINDEX)
		std::cout << "AUTOINDEX on : " << eventFlags << std::endl;
	else 
		std::cout << "AUTOINDEX off : " << eventFlags << std::endl;

	//eventFlags |= AUTOINDEX;

	if (eventFlags & AUTOINDEX)
		std::cout << "AUTOINDEX on : " << eventFlags << std::endl;
	else 
		std::cout << "AUTOINDEX off : " << eventFlags << std::endl;

    if (eventFlags & REDIRECTION)
        std::cout << "REDIRECTION on : " << eventFlags << std::endl;
    else if (!(eventFlags & REDIRECTION))
    {
	    std::cout << "REDIRECTION off : " << eventFlags << std::endl;
	}

	eventFlags |= REDIRECTION;

    if (eventFlags & REDIRECTION)
        std::cout << "REDIRECTION on : " << eventFlags << std::endl;
    else 
    {
	    std::cout << "REDIRECTION off : " << eventFlags << std::endl;
	}



	eventFlags &= ~AUTOINDEX;
	
	if (eventFlags & AUTOINDEX)
		std::cout << "AUTOINDEX on : " << eventFlags << std::endl;
	else 
		std::cout << "AUTOINDEX off : " << eventFlags << std::endl;
	
}
#include "Webserver.hpp"
#include "Response.hpp"

int	main(int argc, char **argv)
{

	if (argc > 2)
	{
		std::cout << "Error: too many arguments." << std::endl;
		return (1);
	}

    std::string configName;

    if (argc == 1)
        configName = std::string("webserv.conf");
    else
    {
        configName = std::string(argv[1]);
    }

	try
	{
        Webserver webserv = Webserver();
		webserv.init(ConfigParser::parseConfig(configName));
        Response::initStatusCodes();
		Response::initContentType();
	    webserv.runServer();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << '\n';
		return (42);
	}
	return (0);
}

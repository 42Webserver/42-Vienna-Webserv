#include "Webserver.hpp"
#include "Response.hpp"

int	main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Error: too many arguments." << std::endl;
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
/*         std::vector<std::string>    m_testvector;
        m_testvector.push_back(".123.123.1:8080");
        ConfigParser::checkValueListen(m_testvector);
        exit(42); */
        Webserver webserv = Webserver(ConfigParser::parseConfig(configName));
        Response::initStatusCodes();
		Response::initContentType();
	    webserv.runServer();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(42);
	}
}

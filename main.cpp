#include "Webserver.hpp"
#include "Response.hpp"
#include "Logger.hpp"

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
		Logger::get().initLogs("logs/error", "logs/access");
        Webserver webserv = Webserver(ConfigParser::parseConfig(configName));
        Response::initStatusCodes();
		Response::initContentType();
	    webserv.runServer();
		Logger::get().closeLogs();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		Logger::get().closeLogs();
	}
}

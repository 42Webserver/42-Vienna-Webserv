#include "Webserver.hpp"

std::map<std::string, std::string> g_status_codes;


void initStatusCodes()
{
    g_status_codes["200"] = "OK";
    g_status_codes["400"] = "Bad Request";
    g_status_codes["404"] = "Not Found";
    g_status_codes["405"] = "Method Not Allowed";
    g_status_codes["414"] = "URI Too Long";
    g_status_codes["500"] = "Internal Server Error";
    g_status_codes["505"] = "HTTP Version not supported";
}

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
        initStatusCodes();
	    webserv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(42);
	}
}

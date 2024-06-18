#include "Webserver.hpp"
#include "Response.hpp"

void testfunc(std::string& a_path)
{
	std::string m_responseBody;
	DIR* dir = opendir(a_path.c_str());
	if (dir == NULL)
		return ;
	std::string uri = a_path;
	std::string root = "/home/nsassenb/Documents/Core/42-Vienna-Webserv/www";
	uri.erase(uri.find(root), root.length());
	m_responseBody.append("<!DOCTYPE html><body><div style=\"display: flex; flex-direction: column; justify-items: center; align-items: center; width: 100%;\"><h1>Auto Index:</h1>");
	struct dirent* de;
	while ((de = readdir(dir)) != NULL)
	{
		m_responseBody += "<a href=\"" + uri + de->d_name + "\">" + uri + de->d_name + "</a>";
		std::cout << "\nName: " << de->d_name << "\nino: " << de->d_ino << "\noff: " << de->d_off << "\nreclen: " << de->d_reclen << "\ntype: " << (int)de->d_type << std::endl;
	}
	m_responseBody.append("</div></body>\r\n");
	std::cout << m_responseBody;
	closedir(dir);
}

int	main(int argc, char **argv)
{
	// std::string path = "/home/nsassenb/Documents/Core/42-Vienna-Webserv/www";
	// testfunc(path);
	// return (0);
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
	    webserv.runServer();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(42);
	}
}

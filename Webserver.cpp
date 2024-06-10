#include "Webserver.hpp"

Webserver::Webserver(void)
{
	Server testSrvr;
	if (testSrvr.initServerSocket(INADDR_ANY, 8080) != -1)
	{
		m_servers.push_back(testSrvr);
		m_polls.addServer(testSrvr);
	}
	if (testSrvr.initServerSocket(INADDR_ANY, 8090) != -1)
	{
		m_servers.push_back(testSrvr);
		m_polls.addServer(testSrvr);
	}
	else
		std::cerr << "fail port 8090" << '\n';
}

Webserver::Webserver(const std::string& configName)
{
	try
	{
		this->readConfigFile(configName);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

Webserver::Webserver(const Webserver& a_other) : m_servers(a_other.m_servers) {}

Webserver&	Webserver::operator=(const Webserver& a_other)
{
	if (this != &a_other)
	{
		m_servers = a_other.m_servers;
	}
	return (*this);
}

Webserver::~Webserver() {}

int Webserver::pollServers(void)
{
	for (std::size_t i = 0; i < m_servers.size(); i++)
	{
		if (m_polls.getPollfdsAt(i).revents & POLLIN)
		{
			int client_socket = m_servers.at(i).acceptNewConnection(m_polls.getPollfdsAt(i).fd);
			if (client_socket == -1)
			{
				std::cerr << "Server Error: could not accept new client" << '\n';
				continue ;
			}
			m_polls.addConnection(client_socket);
		}
/* 		if (m_polls.getPollfdsAt(i).revents & POLLOUT)
			std::cout << "Pollout triggered" << '\n'; */
		if (m_polls.getPollfdsAt(i).revents & POLLERR)
			std::cout << "Socket error.\n";
		if (m_polls.getPollfdsAt(i).revents & POLLHUP)
			std::cout << "Socket hang-up.\n";
		if (m_polls.getPollfdsAt(i).revents & POLLNVAL)
			std::cout << "Invalid request: socket not open.\n";
	}
	return (0);
}

int Webserver::pollClients(void)
{
	for (std::size_t i = m_servers.size(); i < m_polls.getPollfds().size(); i++)
	{
		std::cout << "Socket " << m_polls.getPollfdsAt(i).fd << "	 events: " << m_polls.getPollfdsAt(i).revents << '\n';
		if (m_polls.getPollfdsAt(i).revents & POLLIN)
		{
			std::cout << "Do read/accept request.\n";
			if (m_polls.getConnection(i).reciveRequestRaw() == -1)
				std::cerr << "Mal schaun\n";
			m_polls.getPollfdsAt(i).events |= POLLOUT;
		}
		if (m_polls.getPollfdsAt(i).revents & POLLOUT)
		{
			std::cout << "Pollout triggered\n";
			if (m_polls.getConnection(i).sendResponse() == -1)
				std::cerr << "Error: send\n";
			m_polls.removeConnection(i--); 	//? Sicher
			continue ;						//?
		}
		if (m_polls.getPollfdsAt(i).revents & POLLERR)
			std::cout << "Socket error.\n";
		if (m_polls.getPollfdsAt(i).revents & POLLHUP)
			std::cout << "Socket hang-up.\n";
		if (m_polls.getPollfdsAt(i).revents & POLLNVAL)
			std::cout << "Invalid request: socket not open.\n";
	}
	return (0);
}

int	Webserver::run()
{
	int	pollRet = 0;
	while (pollRet != -1)
	{
		pollRet = poll(m_polls.getPollfds().data(), m_polls.getPollfds().size(), -1);
		if (pollRet == -1)
			std::cerr << "Error: poll error.\n";
		else if (pollRet == 0)
			std::cout << "Poll timeout\n";
		else
		{
			if (pollServers() == -1)
				continue; //try save by clean function?
			if (pollClients() == -1)
				continue; //? überlegen ob das sinn macht. Error handling geschichte
		}
	}
	return (0);
}

void Webserver::replaceWhitespaces(std::string& str)
{
    std::string whitespaces = "\t\r\n\f\v ";

    for (std::string::iterator  it = str.begin(); it != str.end(); ++it)
    {
        if (whitespaces.find(*it) == std::string::npos)
            continue;
        else
        {
            std::string::iterator tmp = it;
            for (; tmp != str.end(); ++tmp)
            {
                if (whitespaces.find(*tmp) == std::string::npos)
                    break;
            }
            str.erase(it + 1, tmp);
            if (*it != ' ')
                *it = ' ';
        }
    }
}

void	Webserver::separateSpecialChars(std::string& str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (i != str.length() - 1 && str.at(i) == ';' && (str.at(i + 1) != ' '))
			str.insert(str.begin() + i + 1, ' ');
		if (str.at(i) == ';' && (i != 0 && str.at(i - 1) != ' '))
			str.insert(str.begin() + i, ' ');
		else if (str.at(i) == '{' && (i != 0 && str.at(i - 1) != ' '))
			str.insert(str.begin() + i, ' ');
		else if (str.at(i) == '}' && (i != 0 && str.at(i - 1) != ' '))
			str.insert(str.begin() + i, ' ');
	}
}

void    Webserver::removeComments(std::string& str)
{
    size_t hashtag;
    if ((hashtag = str.find('#')) != std::string::npos)
    {
        str.erase(hashtag, str.length());
    }
}

bool Webserver::checkDoubleSemicolon(std::vector<std::string> tokens)
{
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (i > 0 && tokens.at(i) == ";" && tokens.at(i - 1) == ";")
		{
			std::cerr << "Double semicolumn" << std::endl;
			return (true);
		}
	}
	return (false);
}

void	Webserver::processLine(std::string& line, std::vector<std::string>& tokens)
{
	if (line.empty())
		return;

	this->removeComments(line);
	this->replaceWhitespaces(line);
	this->separateSpecialChars(line);

	std::stringstream	stream(line);

	std::string current;


	int checkBegin = 0;
	while (std::getline(stream, current, ' '))
	{
		std::cout << "XXX: " << current << std::endl;
		if (current == ";" && checkBegin == 0)
		{
			std::cout << "ERROR 0!!!" << std::endl;
			exit(1);
		}
		if (!current.empty())
			tokens.push_back(current);
		if (!current.empty())
			checkBegin++;
	}

	if (((tokens.back() != ";") && (tokens.back() != "http" && tokens.back() != "Server" && tokens.back() != "{" && tokens.back() != "}")))
	{
		std::cout << "hier: '" << tokens.back() << "'" << std::endl;
		std::cout << "ERROR 1!!!" << std::endl;
		exit(1);
	}
	else if (tokens.back() == ";")
	{
		std::string vorletztes;

		if (tokens.size() >= 2)
			vorletztes = tokens.at(tokens.size() - 2);

		if (vorletztes.empty())
		{
			std::cout << "hier: '" << tokens.back() << "'" << std::endl;
			std::cout << "ERROR 2!!!" << std::endl;
			exit(1);
		}

		if (vorletztes == "http" || vorletztes == "Server" || vorletztes == "{" || vorletztes == "}"  /* || vorletztes == ";" */)
		{
			std::cout << "hier: '" << tokens.back() << "'" << std::endl;
			std::cout << "ERROR 3!!!" << std::endl;
			exit(1);
		}
	}
	if (checkDoubleSemicolon(tokens))
		exit(1);
}

bool	Webserver::checkBraces(std::vector<std::string>& tokens)
{
	std::vector<std::string>::iterator	it;
	int									count = 0;

	for (it = tokens.begin(); it != tokens.end(); ++it)
	{
		if (*it == "{")
			count++;
		else if (*it == "}")
			count--;
	}
	return (count != 0);
}

void	Webserver::readConfigFile(const std::string& file)
{
	std::cout << "<< Starting to read the config-file... >>\n" << std::endl;

	std::ifstream inFile(file.c_str());

	if (!inFile.is_open())
		throw(std::runtime_error("Error: could not open config-file."));

	std::stringstream buffer;
	buffer << inFile.rdbuf();

	std::string line;
	std::getline(buffer, line, '\n');

	if (line.empty())
		throw(std::runtime_error("Error: config-file is empty."));

	std::vector<std::string>	tokens;

	processLine(line, tokens);

	while (std::getline(buffer, line, '\n'))
	{
		try
		{
			processLine(line, tokens);
			// config_raw << line;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}

	if (checkBraces(tokens))
	{
		std::cout << "BRACES-ERROR!" << std::endl;
		exit (1234);
	}

	std::cout << "Result: " << std::endl;
	for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		std::cout << *it << "<EOL>" << std::endl;
	}

	inFile.close();
	exit(42);
}

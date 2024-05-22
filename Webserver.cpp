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

void	Webserver::trimWhitespaces(std::string& str)
{
	std::string	whitespaces = "\t\n\v\f\r ";
	size_t		start = 0;
	size_t		end = str.length() - 1;

	while (start < str.length())
	{
		if (whitespaces.find(str[start]) > whitespaces.length())
			break;
		start++;
	}
	while (end > start)
	{
		if (whitespaces.find(str[end]) > whitespaces.length())
			break;
		end--;
	}
	if (start != str.length())
		str = str.substr(start, end - start + 1);
}

// void	Webserver::replaceWhitespaces(std::string& str, char c)
// {
// 	std::string	whitespaces = "\t\n\v\f\r ";
// 	size_t		start = 0;
// 	size_t		end = str.length() - 1;
// 	size_t		i = 0;

// 	size_t		len = str.length();

// 	while (i < len)
// 	{
// 		if (whitespaces.find(str[i]) == std::string::npos)
// 			i++;
// 		if (str[i] != '\0')
// 		{
// 			start = i;
// 			while ()
// 		}
// 	}

// }

void	Webserver::processLine(std::string line)
{
	std::stringstream	stream(line);

	if (line.empty())
		return;

	// update this function to trim whitespaces to one space in the middle of the string
	this->trimWhitespaces(line);

	//this->replaceWhitespaces(line, ' ');

	std::cout << "line: " << line << std::endl;

	std::string	current;

    while (std::getline(stream, current, ' '))
	{

	}
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

	while (std::getline(buffer, line, '\n'))
	{
		try
		{
			processLine(line);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}

	inFile.close();
}

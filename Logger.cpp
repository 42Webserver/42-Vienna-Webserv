#include "Logger.hpp"

Logger::LogType::LogType() : m_fd(-1) {}

Logger::LogType::LogType(const std::string& a_filePath)
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer [80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "_%Y-%m-%d_%H:%M:%S", timeinfo);

	m_fd = open((a_filePath + buffer + ".log").c_str(), O_CREAT | O_RDWR | O_APPEND, 0666);
	if (m_fd == -1)
		std::cerr << "Error: failed to open \"" << a_filePath << "\" logfile\n";
}

Logger::LogType::LogType(const LogType& a_other) : m_fd(a_other.m_fd) {}

Logger::LogType& Logger::LogType::operator=(const LogType& a_other)
{
	if (this != &a_other)
	{
		if (m_fd != a_other.m_fd)
			closeFile();
		m_fd = a_other.m_fd;
	}
	return (*this);
}

Logger::LogType::~LogType()
{
}

Logger::LogType &Logger::LogType::ts()
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer [80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S | ", timeinfo);
	return(*this << buffer);
}

void Logger::LogType::closeFile(void)
{
	if (m_fd != -1 && m_fd > 2)
		close(m_fd);
	m_fd = -1;
}

bool Logger::LogType::isGood(void) const
{
	return (m_fd != -1);
}

Logger::Logger() : Error(), Access() {}

Logger::~Logger() {}

int	Logger::initLogs(const std::string& errorFileName, const std::string& accessFileName)
{
	Error = LogType(errorFileName);
	Access = LogType(accessFileName);
	
	return (!Error.isGood() || !Access.isGood());
}

void	Logger::closeLogs(void)
{
	Error.closeFile();
	Access.closeFile();
}

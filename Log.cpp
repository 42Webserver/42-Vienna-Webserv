#include "Log.hpp"

Log::LogType Log::Error;
Log::LogType Log::Access;

int Log::LogType::openInit(void)
{
	//add on the file path at the end date and time of start
	//and if date and time already exists add _{num} at the end
	time_t rawtime;
	struct tm* timeinfo;
	char buffer [80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "_%Y-%m-%d_%H:%M:%S", timeinfo);

	m_file.open((m_fileName + buffer + ".log").c_str(), std::ofstream::out | std::ofstream::app);
	if (!m_file.is_open() || !m_file.good())
		throw(std::runtime_error("Error open file"));
	return (0);
}

Log::LogType::LogType()
{
}

Log::LogType::LogType(const std::string &a_filePath) : m_fileName(a_filePath)
{
	//add on the file path at the end date and time of start
	//and if date and time already exists add _{num} at the end
	openInit();
}

Log::LogType::LogType(const LogType &a_other) : m_fileName(a_other.m_fileName)
{
	openInit();
}

Log::LogType &Log::LogType::operator=(const LogType &a_other)
{
	if (this != &a_other)
	{
		m_fileName = a_other.m_fileName;
		openInit();
	}
	return (*this);
}

Log::LogType::~LogType()
{
	m_file.close();
}

Log::LogType &Log::LogType::ts()
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer [80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S | ", timeinfo);
	return(*this << buffer);
}

int Log::Initialize(const std::string &errorLogFileName, const std::string &accesLogFileName)
{
	Error = LogType(errorLogFileName);
	Access = LogType(accesLogFileName);
	return 0;
}

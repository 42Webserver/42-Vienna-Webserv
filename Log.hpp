#ifndef LOG_HPP
# define LOG_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include "Singleton.hpp"

class Log
{
public:
	class LogType
	{
	private:

		std::string		m_fileName;
		std::ofstream	m_file;

		int				openInit(void);

	public:

		LogType();
		LogType(const std::string& a_filePath);
		LogType(const LogType& a_other);
		LogType& operator=(const LogType& a_other);
		~LogType();

		template <typename T>
		LogType& operator<<(const T& msg)
		{
			std::stringstream ss;
			time_t rawtime;
			struct tm* timeinfo;
			char buffer [80];
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			strftime(buffer, 80, "%Y-%m-%d %H:%M:%S | ", timeinfo);
			ss << buffer << msg;
			m_file.write(ss.str().c_str(), ss.str().length());
			return (*this);
		}

	};
private:

	Log(void);
	Log(const Log& a_other);
	Log& operator=(const Log& a_other);
	~Log();

public:
	
	static int Initialize(const std::string& errorLogFileName, const std::string& accesLogFileName);

	static LogType Error;
	static LogType Access;
	// operator Log&(void);
};

#endif // !LOG_HPP

#ifndef LOG_HPP
# define LOG_HPP

#include <fstream>
#include <sstream>
#include <iostream>

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
			std::string	str;
			ss << msg;
			str = ss.str();
			m_file.write(str.c_str(), str.length());
			return (*this);
		}

		LogType& ts();

	};
private:

	Log(void);
	Log(const Log& a_other);
	Log& operator=(const Log& a_other);
	~Log();

public:
	
	static int 		Initialize(const std::string& errorLogFileName, const std::string& accesLogFileName);

	static LogType Error;
	static LogType Access;
	// operator Log&(void);
};

#endif // !LOG_HPP

#ifndef LOGGER_HPP
# define LOGGER_HPP

#include "Singleton.hpp"
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <iostream>

#define LOGF Logger::get()

class Logger : public Singleton<Logger>
{
public:
	class LogType
	{
	private:
		int	m_fd;

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
			write(m_fd, str.c_str(), str.length());
			return (*this);
		}

		LogType&	ts(void);

		void	closeFile(void);
		bool	isGood(void) const;
	};

	Logger();
	~Logger();
	
	int		initLogs(const std::string& errorFileName, const std::string& accesFileName);
	void	closeLogs(void);

	LogType Error;
	LogType Access;
};

#endif //!LOGGER_HPP

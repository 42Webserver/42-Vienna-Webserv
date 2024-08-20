#ifndef DEBUG_HPP
# define DEBUG_HPP

#include <ctime>
#include <iomanip>
#include <iostream>

#define TERMC_RED "\033[91m"
#define TERMC_DARKRED "\033[31m"
#define TERMC_DARKMAG "\033[35m"
#define TERMC_GREEN "\033[92m"
#define TERMC_DARKGREEN "\033[32m"
#define TERMC_BLUE "\033[94m"
#define TERMC_ORANGE "\033[33m"
#define TERMC_CYAN "\033[36m"

#ifdef DEBUG
#define LOG(msg) std::cout << msg << '\n';
#define LOGC(colour, msg) { \
	std::time_t rawtime; \
	struct std::tm* timeinfo; \
	char buffer [80]; \
	std::time(&rawtime); \
	timeinfo = std::localtime(&rawtime); \
	std::strftime(buffer,80, "%Y-%m-%d %H:%M:%S | ", timeinfo); \
	std::cout << buffer << colour << msg << "\033[39m" << '\n'; \
} 
#define LOG_INFO(msg) LOGC(TERMC_BLUE, msg)
#define LOG_WARNING(msg) LOGC(TERMC_ORANGE, msg)
#define LOG_ERROR(msg) LOGC(TERMC_RED, msg)
#else
#define LOG(msg){}
#define LOGC(colour, msg){}
#define LOG_INFO(msg) {}
#define LOG_WARNING(msg) {}
#define LOG_ERROR(msg) {}
#endif

#endif // !DEBUG_HPP

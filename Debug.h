#ifndef DEBUG_HPP
# define DEBUG_HPP

#include <ctime>
#include <iomanip>
#include <iostream>

#define TERMC_RED "\033[91m"
#define TERMC_GREEN "\033[92m"
#define TERMC_DARKGREEN "\033[32m"
#define TERMC_BLUE "\033[94m"
#define TERMC_ORANGE "\033[33m"
#define TERMC_CYAN "\033[36m"

#ifdef DEBUG
#define LOG(msg) std::cout << msg << '\n';
#define LOGC(colour, msg) { \
	time_t rawtime; \
	struct tm* timeinfo; \
	char buffer [80]; \
	time(&rawtime); \
	timeinfo = localtime(&rawtime); \
	strftime(buffer,80, "%Y-%m-%d %H:%M:%S | ", timeinfo); \
	std::cout << buffer << colour << msg << "\033[39m" << '\n'; \
} 
#else
#define LOG(msg){}
#define LOGC(colour, msg){}
#endif

#endif // !DEBUG_HPP

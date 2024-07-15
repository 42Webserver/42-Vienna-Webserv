#ifndef TIMER_H
# define TIMER_H

#include <sys/types.h>
#include <ctime>

class Timer
{
private:
	std::time_t	m_startTime;
public:
	Timer(void);
	Timer(const Timer& other);
	Timer& operator=(const Timer& other);
	~Timer();

	void		resetTime(void);
	std::time_t	getRunningTime(void) const;
	bool		isOver(std::time_t a_seconds);
};

#endif // !TIMER_H
#include "Timer.hpp"

Timer::Timer(void)
{
	m_startTime = std::time(NULL);
}

Timer::Timer(const Timer &other)
{
	m_startTime = other.m_startTime;
}

Timer &Timer::operator=(const Timer &other)
{
	if (this != &other)
	{
		m_startTime = other.m_startTime;
	}
	return (*this);
}

Timer::~Timer() {}

void Timer::resetTime(void)
{
	m_startTime = std::time(NULL);
}

std::time_t Timer::getRunningTime(void) const
{
	return (std::time(NULL) - m_startTime);
}

bool Timer::isOver(std::time_t a_seconds)
{
	return ((std::time(NULL) - m_startTime) > a_seconds);
}

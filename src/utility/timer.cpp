#include "timer.h"
#include <chrono>
#include <cmath>
#include <iostream>

static long long sysTime()
{
	// Get the current system time in milliseconds since epoch
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count() / 1000ll;
}

Timer::Timer(float tps)
{
	setTickRate(tps);
	m_lastSync = sysTime();
}

void Timer::update()
{
	long long time = sysTime();
	float partialTicks = (time - m_lastSync) / m_tickLength;

	m_lastSync = time;

	m_alpha += partialTicks;
	m_elapsedTicks = static_cast<int>(floor(m_alpha));
	m_alpha -= static_cast<float>(m_elapsedTicks);
}

void Timer::setTickRate(float tps)
{
	this->m_tps = tps;
	m_tickLength = 1000.0f / tps;
}
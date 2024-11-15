#pragma once

class Timer
{
public:
	Timer(float tps);
	void update();
	void setTickRate(float tps);

	const int getTickCount() const { return m_elapsedTicks; }
	const float getAlpha() const { return m_alpha; }

private:
	int m_elapsedTicks = 0;

	float m_tps = 0.0f;
	float m_tickLength = 0.0f;
	long long m_lastSync = 0;
	
	float m_alpha = 0.0f;
};
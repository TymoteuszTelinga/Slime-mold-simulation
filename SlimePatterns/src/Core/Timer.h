#pragma once

#include <cstdint>

class Timer
{
public:
	Timer();
	~Timer();

	float ElapsedMiliseconds();
	void Clear();

private:

	uint32_t m_QueryId;
};
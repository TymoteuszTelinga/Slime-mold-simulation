#include "Timer.h"
#include <GL/glew.h>

Timer::Timer()
{
	glGenQueries(1, &m_QueryId);
	glBeginQuery(GL_TIME_ELAPSED, m_QueryId);
}

Timer::~Timer()
{
	glDeleteQueries(1, &m_QueryId);
}

float Timer::ElapsedMiliseconds()
{
	glEndQuery(GL_TIME_ELAPSED);

	int32_t Done = 0;
	while (!Done)
	{
		glGetQueryObjectiv(m_QueryId, GL_QUERY_RESULT_AVAILABLE, &Done);
	}

	uint32_t GPUTime = 0;
	glGetQueryObjectuiv(m_QueryId, GL_QUERY_RESULT, &GPUTime);
	glBeginQuery(GL_TIME_ELAPSED, m_QueryId);

	return (float)GPUTime / 1000000.0f;
}

void Timer::Clear()
{
	glEndQuery(GL_TIME_ELAPSED);
	glBeginQuery(GL_TIME_ELAPSED, m_QueryId);
}

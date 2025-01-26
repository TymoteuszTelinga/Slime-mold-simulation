#pragma once

#include "Core/Application.h"
#include "Core/Timer.h"

class Simulation : public Application
{
public:
	Simulation(const ApplicationSpecification& spec);
	~Simulation() {};

	virtual void OnEvent(Event& e) override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnRender() override;

private:

	void InitSimulation(float* EnvironmentData);
	void ParseImage(float* EnvironmentData);

	void InitAgents();

private:

	Scope<Timer> m_Timer;
	bool bPouse = false;
	bool bNextFrame = false;
	bool bVsync = true;

	std::string m_EnvironmentPath;

	std::uint32_t m_S1Points, m_S2Points, m_S3Points;
	std::vector<vec3> ValidPositions;

	//Debug info
	float m_FrameTime = 0.0f;
	float m_CountTime = 0.0f;
	int m_FPS = 0;
	int m_Count = 0;

	float m_AgentTime = 0.0f;
	float m_DifiuseTime = 0.0f;
	float m_EvaporateTime = 0.0f;

	vec3 m_ColorLow  = vec3(0.231, 0.341, 0.000);
	vec3 m_ColorHigh = vec3(0.784, 1.000, 0.321);
};


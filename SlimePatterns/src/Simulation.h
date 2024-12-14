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

	void InitSimulation();

private:

	Scope<Timer> m_Timer;
	bool bPouse = false;
	bool bNextFrame = false;
	bool bVsync = true;

	//Debug info
	float m_FrameTime = 0.0f;
	float m_CountTime = 0.0f;
	int m_FPS = 0;
	int m_Count = 0;

	float m_AgentTime = 0.0f;
	float m_DifiuseTime = 0.0f;
	float m_EvaporateTime = 0.0f;

};


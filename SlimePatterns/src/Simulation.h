#pragma once

#include "Core/Application.h"

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

	bool bPouse = false;
	bool bNextFrame = false;
	bool bVsync = true;

	//Debug info
	float m_FrameTime = 0.0f;
	float m_CountTime = 0.0f;
	int m_FPS = 0;
	int m_Count = 0;

};


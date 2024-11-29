#pragma once

#include <GL/glew.h>
#include <string>
#include "Base.h"
#include "Window.h"

struct ApplicationSpecification
{
	std::string Name = "Application";
	uint32_t Width = 1280;
	uint32_t Height = 720;
	uint32_t MinUpdateFrameRate = 30;
};

/*
* Class representing the application responsible for forwarding events and handling the main program loop
*/
class Application
{
public:
	Application(const ApplicationSpecification& spec);
	~Application();

	static Application& Get() { return *s_Instance;}

	Window& GetWindow() const { return *m_Window; };

	virtual void Run();
	/*
	* Function that allows to react to events that occurred while the application is running
	* @param e event that occurred
	*/
	virtual void OnEvent(Event& e) {};
	/*
	* function representing a computational step of the program logic
	* @param DeltaTime time expressed in seconds between individual function calls
	*/
	virtual void OnUpdate(float DeltaTime) {};
	/*
	* Represents the rendering step of a current frame
	*/
	virtual void OnRender() {};

protected:
	void ResetDelta();

private:
	void OnEventApp(Event& e);
	bool ResizeVieport(WindowResizeEvent& e);
	bool WindowMoved(WindowMovedEvent& e);

private:
	ApplicationSpecification m_Specification;
	Scope<Window> m_Window;

	bool m_Runing = false;
	float m_DeltaTime = 0.0f;
	float m_LastTime = 0.0f;
	bool m_Minimized = false;

private:
	static Application* s_Instance;
};

